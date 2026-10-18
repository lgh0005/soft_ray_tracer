#pragma once

namespace bdt
{
	BDT_CLASS_PTR(InstanceBuffer)

	class InstanceBuffer
	{
		BDT_DISABLE_COPY(InstanceBuffer)
		BDT_DISABLE_MOVE(InstanceBuffer)

	public:
		~InstanceBuffer();
		static InstanceBufferUPtr Create();

		void Update(const void* data, uint32 byteSize);
		void Bind(uint32 slot) const;
		static void Unbind(uint32 slot);
		uint32 GetSize() const { return m_size; }
		uint32 GetCapacity() const { return m_capacity; }

	protected:
		InstanceBuffer();
		bool Init();

		uint32 CreateBuffer();
		void SetCapacity(uint32 requiredBytes);

	private:
		uint32 m_instBufferID{ 0 };
		uint32 m_size{ 0 };
		uint32 m_capacity{ 0 };
	};
}
