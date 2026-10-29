#pragma once

// 표준 라이브러리 의존성
// 공통 타입 별칭을 정의하는 데 필요한 C++ 표준 라이브러리 헤더를 포함합니다.
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <list>
#include <deque>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <queue>
#include <stack>
#include <utility>
#include <tuple>
#include <span>
#include <memory>
#include <functional>
#include <optional>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <atomic>
#include <limits>
#include <chrono>
#include <ratio>
#include <source_location>
#include <future>
#include <filesystem>
#include <ios>
#include <istream>
#include <ostream>
#include <fstream>
#include <sstream>

// 수학 라이브러리 의존성
// GLM에서 제공하는 벡터, 행렬 및 사원수 타입을 사용하기 위한 헤더를 포함합니다.
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace bdt
{
	// 기본 자료형 별칭
// 고정 크기 정수형과 플랫폼에 따른 크기 표현 자료형의 별칭을 정의합니다.
	using int8 = std::int8_t;
	using int16 = std::int16_t;
	using int32 = std::int32_t;
	using int64 = std::int64_t;
	using uint8 = std::uint8_t;
	using uint16 = std::uint16_t;
	using uint32 = std::uint32_t;
	using uint64 = std::uint64_t;
	using usize = std::size_t;
	using null = std::nullptr_t;

	// 수학 자료형 별칭
	// GLM에서 제공하는 벡터, 행렬 및 사원수 타입의 별칭을 정의합니다.
	using vec2 = glm::vec2;
	using vec3 = glm::vec3;
	using vec4 = glm::vec4;
	using ivec2 = glm::ivec2;
	using ivec3 = glm::ivec3;
	using ivec4 = glm::ivec4;
	using uvec2 = glm::uvec2;
	using uvec3 = glm::uvec3;
	using uvec4 = glm::uvec4;
	using dvec2 = glm::dvec2;
	using dvec3 = glm::dvec3;
	using dvec4 = glm::dvec4;
	using mat2 = glm::mat2;
	using mat3 = glm::mat3;
	using mat4 = glm::mat4;
	using quat = glm::quat;
	template<typename T> inline constexpr T Epsilon = glm::epsilon<T>();

	// 컨테이너 및 문자열 타입 별칭
	// 표준 컨테이너, 튜플, 연속된 메모리 영역을 참조하는 Span 및 문자열 타입의 별칭을 정의합니다.
	template<typename T>                        using List = std::vector<T>;
	template<typename T, usize Size>            using Array = std::array<T, Size>;
	template<typename T>                        using LinkedList = std::list<T>;
	template<typename T>                        using Deque = std::deque<T>;
	template<typename TKey, typename TValue>    using Dictionary = std::unordered_map<TKey, TValue>;
	template<typename TKey, typename TValue>    using SortedDictionary = std::map<TKey, TValue>;
	template<typename T>                        using HashSet = std::unordered_set<T>;
	template<typename T>                        using SortedSet = std::set<T>;
	template<typename T>                        using Queue = std::queue<T>;
	template<typename T>                        using Stack = std::stack<T>;
	template<typename T>                        using PriorityQueue = std::priority_queue<T>;
	template<typename TFirst, typename TSecond> using Pair = std::pair<TFirst, TSecond>;
	template<typename... TArgs>                 using Tuple = std::tuple<TArgs...>;
	template<typename T>                        using Span = std::span<T>;
	template<typename T>                        using ReadOnlySpan = std::span<const T>;
	using cstr = const char*;
	using String = std::string;
	using StringView = std::string_view;
	using ByteString = std::u8string;
	using ByteStringView = std::u8string_view;

	// 객체 수명 관리 타입 별칭
	// shared_ptr로 관리되는 객체가 자기 자신을 공유 소유할 수 있도록 지원합니다.
	template<typename T> using Self = std::enable_shared_from_this<T>;

	// 함수 및 콜백 타입 별칭
	// 반환값이 있는 함수와 반환값이 없는 함수의 호출 가능한 객체 타입을 정의합니다.
	template<typename TResult, typename... TArgs> using Func = std::function<TResult(TArgs...)>;
	template<typename... TArgs>                   using Action = std::function<void(TArgs...)>;

	// 선택적 값 타입 별칭
	// 값이 존재하거나 존재하지 않을 수 있는 자료형을 정의합니다.
	template<typename T>
	using Nullable = std::optional<T>;

	// 비동기 작업 결과 타입 별칭
	// 비동기 작업의 결과를 전달하고 여러 곳에서 공유할 수 있도록 지원합니다.
	template<typename T> using Promise = std::promise<T>;
	template<typename T> using Future = std::future<T>;
	template<typename T> using SharedFuture = std::shared_future<T>;

	// 시간 관련 타입 별칭
	// 단조 시계, 밀리초 단위 및 시간 지점과 시간 간격을 표현하는 타입을 정의합니다.
	using SteadyClock = std::chrono::steady_clock;
	using Milli = std::milli;

	template<typename TClock, typename TDuration = typename TClock::duration>
	using TimePoint = std::chrono::time_point<TClock, TDuration>;

	template<typename TRep, typename TPeriod = std::ratio<1>>
	using Duration = std::chrono::duration<TRep, TPeriod>;

	// 공통 시간 타입 별칭
	// 단조 시계의 시간 지점과 정수 및 부동소수점 기반 밀리초 시간 간격을 정의합니다.
	using SteadyTimePoint = SteadyClock::time_point;
	using Milliseconds = std::chrono::milliseconds;
	using MillisecondDuration = Duration<double, Milli>;

	// 진단 정보 타입 별칭
	// 코드의 파일명, 줄 번호 및 함수명 등의 호출 위치 정보를 표현하는 타입을 정의합니다.
	using SourceLocation = std::source_location;

	// 스레드 및 동기화 타입 별칭
	// 스레드, 뮤텍스, 원자적 변수 및 잠금 관리 객체의 타입을 정의합니다.
	using Thread = std::jthread;
	using Mutex = std::mutex;
	using RWLock = std::shared_mutex;
	using ConditionVariable = std::condition_variable;

	template<typename T>      using Atomic = std::atomic<T>;
	template<typename TMutex> using LockGuard = std::lock_guard<TMutex>;
	template<typename TMutex> using UniqueLock = std::unique_lock<TMutex>;
	template<typename TMutex> using SharedLock = std::shared_lock<TMutex>;

	// 자료형의 수치 범위
	// 자료형에서 표현 가능한 최솟값과 최댓값을 컴파일 타임 상수로 정의합니다.
	template<typename T> inline constexpr T MinValue = std::numeric_limits<T>::lowest();
	template<typename T> inline constexpr T MaxValue = std::numeric_limits<T>::max();

	// 파일 시스템 
	// [설명]
	namespace fs = std::filesystem;
	using Path = fs::path;
	using FileStatus = fs::file_status;
	using FileType = fs::file_type;
	using DirectoryEntry = fs::directory_entry;
	using DirectoryIterator = fs::directory_iterator;
	using RecursiveDirectoryIterator = fs::recursive_directory_iterator;

	// 스트림 및 파일 입출력
	using InputStream = std::istream;
	using OutputStream = std::ostream;
	using IOStream = std::iostream;

	using InputFileStream = std::ifstream;
	using OutputFileStream = std::ofstream;
	using FileStream = std::fstream;
	using InputStringStream = std::istringstream;
	using OutputStringStream = std::ostringstream;
	using StringStream = std::stringstream;

	using StreamSize = std::streamsize;
	using StreamPosition = std::streampos;
	using StreamOffset = std::streamoff;
	using OpenMode = std::ios::openmode;
}