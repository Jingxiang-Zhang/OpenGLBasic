#pragma once
#include <functional>
#include <vector>
#include <string>
#include <iostream>

namespace test {

	class Test {
	public:
		Test() {}
		virtual ~Test() {}

		virtual void OnUpdate(float deltaTime) {}
		// if set virtual void OnUpdate(float deltaTime) = 0, then it is a pure virtual function
		// you mush override it in the derived class
		virtual void OnRender() {}
		virtual void OnImGuiRender() {}
	};

	class TestMenu : public Test {
	public:
		TestMenu(Test*& currentTestPointer);

		void OnImGuiRender() override;

		template<typename T>
		void RegisterTest(const std::string& name) {
			std::cout << "Registering test " << name << std::endl;
			m_Test.push_back(std::make_pair(name, []() {
				return new T();
			}));
		}

	private:
		Test*& m_CurrentTest;
		// a vector, which contain pair of itmes
		// for each item, it got a string for name, and a function pointer
		// the function pointer point to a function that take no parameter, and return 
		// a Test object pointer.
		std::vector<std::pair<std::string, std::function<Test* ()>>> m_Test;
	};

}