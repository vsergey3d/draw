#include "batcher.h"
#include <image.h>
#include <error.h>
#include <algorithm>
#include <cstring>

namespace draw {

inline uint32_t typeByteSize(GLenum type) {

	switch (type) {
	case GL_UNSIGNED_BYTE: return 1;
	case GL_FLOAT: return 4;
	}
	return 0;
}

inline void bindAttribute(GLuint location, GLenum type, bool normalized,
	uint32_t size, uint32_t stride, uint32_t& offset, bool perInstance = false) {

	glEnableVertexAttribArray(location);

	auto norm = GLboolean(normalized ? GL_TRUE : GL_FALSE);
	glVertexAttribPointer(location, size, type, norm, stride, (char*)0 + offset);
	offset += typeByteSize(type) * size;

	if (perInstance)
		glVertexAttribDivisor(location, 1);
}

BatcherImpl::~BatcherImpl() {

	gl::setCurrentContext();

	deleteBuffers();
	deleteProgram();
}

bool BatcherImpl::init(const InstanceData& data, Error::Code& errorCode,
	std::string& errorDesc) {

	gl::setCurrentContext();

	if (!createBuffers(data, errorCode) ||
		!createProgram(data, errorCode, errorDesc)) {
		return false;
	}
	blending_ = data.blending;
	return true;
}

bool BatcherImpl::createBuffers(const InstanceData& data, Error::Code& errorCode) {

	static const auto kMaxIndex = std::numeric_limits<Index>::max() - 1;

	errorCode = Code::NoError;
	if (data.vertexCount <= 0 || data.vertexCount >= kMaxIndex || !data.vertices) {
		errorCode = Code::InvalidBatcherVertexData;
		return false;
	}
	indexCount_ = data.indexCount;
	if (indexCount_ < 1 || !data.indices) {
		errorCode = Code::InvalidBatcherIndexData;
		return false;
	}
	glGenBuffers(1, &vertexBuffer_);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * data.vertexCount,
		data.vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &indexBuffer_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Index) * indexCount_,
		data.indices, GL_STATIC_DRAW);

	glGenBuffers(1, &instanceBuffer_);
	resizeDataBuffer(kDataInitCapacity);

	ASSERT(gl::checkError());
	return true;
}

void BatcherImpl::resizeDataBuffer(uint32_t size) {

	gl::setCurrentContext();

	glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Instance) * size, nullptr, GL_DYNAMIC_DRAW);
	ASSERT(gl::checkError());

	dataBuffer_.resize(size);
}

void BatcherImpl::deleteBuffers() {

	glDeleteBuffers(1, &vertexBuffer_);
	glDeleteBuffers(1, &indexBuffer_);
	glDeleteBuffers(1, &instanceBuffer_);
}

bool BatcherImpl::compileShader(GLuint shader, const char* source, std::string& log) {

	GLint length = strlen(source);
	glShaderSource(shader, 1, &source, &length);
	glCompileShader(shader);

	log.clear();
	return gl::checkShader(shader, log);
}

bool BatcherImpl::createProgram(const InstanceData& data, Error::Code& errorCode,
	std::string& errorDesc) {

	errorCode = Code::NoError;
	errorDesc.clear();

	vertexShader_ = glCreateShader(GL_VERTEX_SHADER);
	if (!compileShader(vertexShader_, data.vertexShader, errorDesc)) {
		errorCode = Code::InvalidBatcherVertexShader;
		return false;
	}
	fragmentShader_ = glCreateShader(GL_FRAGMENT_SHADER);
	if (!compileShader(fragmentShader_, data.fragmentShader, errorDesc)) {
		errorCode = Code::InvalidBatcherFragmentShader;
		return false;
	}
	program_ = glCreateProgram();
	glAttachShader(program_, vertexShader_);
	glAttachShader(program_, fragmentShader_);
	glLinkProgram(program_);
	if (!gl::checkProgram(program_, errorDesc)) {
		errorCode = Code::InvalidBatcherProgram;
		return false;
	}
	attribs_.pos = glGetAttribLocation(program_, "pos");
	attribs_.uv = glGetAttribLocation(program_, "uv");
	attribs_.posFrame = glGetAttribLocation(program_, "posFrame");
	attribs_.uvFrame = glGetAttribLocation(program_, "uvFrame");
	attribs_.color = glGetAttribLocation(program_, "color");
	uniforms_.screenFrame = glGetUniformLocation(program_, "screenFrame");
	uniforms_.atlas = glGetUniformLocation(program_, "atlas");
	ASSERT(gl::checkError());
    return true;
}

void BatcherImpl::deleteProgram() {

	glDeleteShader(vertexShader_);
	glDeleteShader(fragmentShader_);
	glDeleteProgram(program_);
}

Instance* BatcherImpl::add(uint32_t order, ImagePtr atlas) {

	auto& batch = batches_[order][atlas];

	batch.emplace_back(std::unique_ptr<Instance>(new Instance()));
	if (batch.size() > dataBuffer_.size())
		resizeDataBuffer(dataBuffer_.size() * kDataGrowthFactor);

	return batch.back().get();
}

void BatcherImpl::remove(uint32_t order, ImagePtr atlas, Instance* instance) {

	auto itOrder = batches_.find(order);
	ASSERT(itOrder != batches_.end() && "order is not exist");

	auto& byAtlas = itOrder->second;
	auto itAtlas = byAtlas.find(atlas);
	ASSERT(itAtlas != byAtlas.end() && "atlas is not exist");
	
	auto& batch = itAtlas->second;
	auto it = std::find_if(std::begin(batch), std::end(batch),
		[&instance](const InstancePtr& e) { return instance == e.get(); });
	ASSERT(it != batch.end() && "instance is not exist");

	auto index = std::distance(std::begin(batch), it);
	batch[index] = std::move(batch[batch.size() - 1]);
	batch.resize(batch.size() - 1);

	if (batch.empty()) {
		byAtlas.erase(itAtlas);
		if (byAtlas.empty())
			batches_.erase(itOrder);
	}
}

void BatcherImpl::screenSize(uint32_t width, uint32_t height) {

	screenFrame_.z = 2.0f / std::max(1u, width);
	screenFrame_.w = 2.0f / std::max(1u, height);
}

uint32_t BatcherImpl::draw() {

	gl::setCurrentContext();

	if (!blending_)
		glDisable(GL_BLEND);
	else {
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	glUseProgram(program_);
	glUniform4fv(uniforms_.screenFrame, 1, &screenFrame_.x);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer_);

	auto offset = 0u, stride = sizeof(Vertex);
	bindAttribute(attribs_.pos, GL_FLOAT, false, 2, stride, offset);
	bindAttribute(attribs_.uv, GL_FLOAT, false, 2, stride, offset);

	auto total = 0u;
	for (auto& itOrder : batches_) {
		for (auto& itAtlas : itOrder.second) {
			auto count = bindBatch(itAtlas.first, itAtlas.second);
			glDrawElementsInstanced(GL_TRIANGLES, indexCount_,
				GL_UNSIGNED_SHORT, 0, count);
			total += count;
		}
	}
	ASSERT(gl::checkError());
	return total;
}

uint32_t BatcherImpl::bindBatch(ImagePtr atlas, const Batch& batch) {

	if (uniforms_.atlas != -1) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,
			atlas ? static_cast<ImageImpl*>(atlas.get())->handle() : 0);
		glUniform1i(uniforms_.atlas, 0);
	}
	auto count = 0u;
	for (auto& instance : batch)
		dataBuffer_[count++] = *instance;

	glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer_);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Instance) * count, &dataBuffer_[0]);

	auto offset = 0u, stride = sizeof(Instance);
	bindAttribute(attribs_.posFrame, GL_FLOAT, false, 4, stride, offset, true);
	bindAttribute(attribs_.uvFrame, GL_FLOAT, false, 4, stride, offset, true);
	bindAttribute(attribs_.color, GL_UNSIGNED_BYTE, true, 4, stride, offset, true);

	return count;
}

BatcherPtr makeBatcher(const InstanceData& data, Error::Code& errorCode,
	std::string& errorDesc) {

	auto batcher = std::make_shared<BatcherImpl>();
	if (!batcher->init(data, errorCode, errorDesc)) {
		return BatcherPtr();
	}
	return batcher;
}

BatcherPtr makeBatcher(const InstanceData& data) {

	auto errorCode = Code::NoError;
	std::string errorDesc;
	auto batcher = makeBatcher(data, errorCode, errorDesc);
	if (!batcher) {
		throw error(errorCode, errorDesc);
	}
	return batcher;
}

} // namespace draw