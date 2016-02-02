#pragma once
#include <draw.h>
#include <gl.h>
#include <vector>
#include <map>
#include <unordered_map>

#if defined(_MSC_VER)
#pragma warning(disable: 4503)
#endif

namespace draw {

struct Vector4 {

	float x {0.0f};
	float y {0.0f};
	float z {0.0f};
	float w {0.0f};

	Vector4() = default;
	Vector4(float x, float y, float z, float w) :
		x(x), y(y), z(z), w(w) {}
};

struct Instance {

	Vector4 posFrame;
	Vector4 uvFrame;
	uint32_t color {0xFFFFFFFF};
};

class BatcherImpl final : public Batcher {

public:
	BatcherImpl() = default;
	virtual ~BatcherImpl();

	BatcherImpl(const BatcherImpl&) = delete;
	BatcherImpl& operator = (const BatcherImpl&) = delete;

	bool init(const InstanceData& data, Error::Code& errorCode,
		std::string& errorDesc);

	Instance* add(uint32_t order, ImagePtr atlas);
	void remove(uint32_t order, ImagePtr atlas, Instance* instance);

	virtual void screenSize(uint32_t width, uint32_t height) final;
	virtual uint32_t draw() final;

private:
	bool createBuffers(const InstanceData& data, Error::Code& errorCode);
	void resizeDataBuffer(uint32_t size);
	void deleteBuffers();

	bool compileShader(GLuint shader, const char* source, std::string& log);
	bool createProgram(const InstanceData& data, Error::Code& errorCode,
		std::string& errorDesc);
	void deleteProgram();

	using InstancePtr = std::unique_ptr<Instance>;
	using Batch = std::vector<InstancePtr>;

	uint32_t bindBatch(ImagePtr atlas, const Batch& batch);

	std::map<uint32_t,
		std::unordered_map<ImagePtr,
			Batch>> batches_;

	static const uint32_t kDataInitCapacity {1000};
	static const uint32_t kDataGrowthFactor {2};
	std::vector<Instance> dataBuffer_;

	GLuint instanceBuffer_ {0};
	GLuint vertexBuffer_ {0};
	GLuint indexBuffer_ {0};
	uint32_t indexCount_ {0};

	GLuint vertexShader_ {0};
	GLuint fragmentShader_ {0};
	GLuint program_ {0};

	struct Attribs {

		GLint pos {0};
		GLint uv {0};
		GLint posFrame {0};
		GLint uvFrame {0};
		GLint color {0};
	};
	Attribs attribs_;

	struct Uniforms {

		GLint atlas {0};
		GLint screenFrame {0};
	};
	Uniforms uniforms_;

	Vector4 screenFrame_ {-1.0f, -1.0f, 1.0f, 1.0f};
	bool blending_ {false};
};

} // namespace draw

#if defined(_MSC_VER)
#pragma warning(disable: 4503)
#endif