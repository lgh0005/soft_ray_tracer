#pragma once

namespace bdt
{
	BDT_CLASS_PTR(MyWorld)

	class MyWorld : public World
	{
		using Super = World;

	public:
		~MyWorld() override;
		static MyWorldUPtr Create(const String& tag = "MyWorld");
		void Update(float deltaTime) override;

	protected:
		void LoadResources() override;
		void CreateActors() override;

	private:
		explicit MyWorld(const String& tag);
		bool Init();
	};
}
