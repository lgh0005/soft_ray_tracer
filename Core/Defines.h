#pragma once

// 클래스 포인터 타입 선언
// 클래스를 전방 선언하고 unique_ptr 및 shared_ptr 기반의 스마트 포인터 별칭을 정의합니다.
#define BDT_CLASS_PTR(className)                            \
    class className;                                        \
    using className##UPtr = std::unique_ptr<className>;     \
    using className##Ptr  = std::shared_ptr<className>;

// 복사 연산 비활성화
// 복사 생성자와 복사 대입 연산자를 삭제하여 객체의 복사를 방지합니다.
#define BDT_DISABLE_COPY(classType)                         \
    classType(const classType&) = delete;                   \
    classType& operator=(const classType&) = delete;

// 이동 연산 비활성화
// 이동 생성자와 이동 대입 연산자를 삭제하여 객체의 이동을 방지합니다.
#define BDT_DISABLE_MOVE(classType)                         \
    classType(classType&&) = delete;                        \
    classType& operator=(classType&&) = delete;

// 싱글턴 클래스 선언
// 객체의 복사 및 이동을 금지하고, 프로그램에서 단일 인스턴스에 접근할 수 있도록 정의합니다.
#define BDT_DECLARE_SINGLE(classType)                       \
private:                                                    \
    BDT_DISABLE_COPY(classType)                             \
    BDT_DISABLE_MOVE(classType)                             \
                                                            \
    classType();                                            \
    ~classType();                                           \
                                                            \
public:                                                     \
    static classType& Instance()                            \
    {                                                       \
        static classType s_instance;                        \
        return s_instance;                                  \
    }

// 프로젝트 내 OpenGL 관련 상수
#define BDT_OPENGL_MAJOR_VERSION 4
#define BDT_OPENGL_MINOR_VERSION 6
#define BDT_STD140_BASE_ALIGNMENT    1 << 4
#define BDT_STD430_BASE_ALIGNMENT    1 << 4