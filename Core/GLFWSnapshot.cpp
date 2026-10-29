#include "pch.h"
#include "GLFWSnapshot.h"

namespace bdt
{
    bool GLFWSnapshot::Fail(String* error, cstr message)
    {
        if (error) *error = message;
        return false;
    }

    void GLFWSnapshot::Put(Array<uint8, 54>& header, usize offset, uint32 value, usize bytes)
    {
        for (usize i = 0; i < bytes; ++i)
            header[offset + i] = static_cast<uint8>(value >> (8 * i));
    }

    bool GLFWSnapshot::Save(GLFWwindow* window, const String& fileName, String* error)
    {
        // 1. 이전 오류를 지우고 윈도우 컨텍스트와 파일명 검사
        if (error) error->clear();
        if (!window || glfwGetCurrentContext() != window)
            return Fail(error, "The snapshot window context must be current on this thread.");
        if (fileName.empty()) 
            return Fail(error, "The snapshot file name is empty.");

        // 2. 프레임버퍼 크기, 최소화 여부, 이중 버퍼 지원 여부 검사
        int32 width = 0, height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        if (width <= 0 || height <= 0 || glfwGetWindowAttrib(window, GLFW_ICONIFIED))
            return Fail(error, "The snapshot framebuffer is empty or minimized.");
        if (!glfwGetWindowAttrib(window, GLFW_DOUBLEBUFFER))
            return Fail(error, "The snapshot requires a double-buffered window.");

        // 3. BMP의 4바이트 행 정렬을 반영해 이미지 크기 계산
        const uint64 stride = (static_cast<uint64>(width) * 3 + 3) & ~uint64(3);
        const uint64 imageSize = stride * static_cast<uint64>(height);

        // 4. OpenGL 읽기 크기와 BMP 파일 크기의 허용 범위 검사
        if (imageSize > static_cast<uint64>(MaxValue<int32>) - 54)
            return Fail(error, "The snapshot framebuffer is too large.");

        // 5. 픽셀을 받을 CPU 버퍼를 준비하고 기존 OpenGL 읽기 및 패킹 상태 보관
        List<uint8> pixels(static_cast<usize>(imageSize), 0);
        GLint framebuffer = 0, readBuffer = 0, packBuffer = 0;
        GLint alignment = 0, rowLength = 0, skipRows = 0, skipPixels = 0;
        glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &framebuffer);
        glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &packBuffer);
        glGetIntegerv(GL_PACK_ALIGNMENT, &alignment);
        glGetIntegerv(GL_PACK_ROW_LENGTH, &rowLength);
        glGetIntegerv(GL_PACK_SKIP_ROWS, &skipRows);
        glGetIntegerv(GL_PACK_SKIP_PIXELS, &skipPixels);

        // 6. 기본 프레임버퍼의 백버퍼를 선택하고 BMP에 맞는 CPU 전송 상태 설정
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glGetIntegerv(GL_READ_BUFFER, &readBuffer);
        glReadBuffer(GL_BACK);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
        glPixelStorei(GL_PACK_ALIGNMENT, 4);
        glPixelStorei(GL_PACK_ROW_LENGTH, 0);
        glPixelStorei(GL_PACK_SKIP_ROWS, 0);
        glPixelStorei(GL_PACK_SKIP_PIXELS, 0);

        // 7. 백버퍼의 픽셀을 BGR 형식으로 CPU 버퍼에 동기적으로 읽기
        glReadnPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, static_cast<GLsizei>(imageSize), pixels.data());

        // 8. 기본 프레임버퍼의 읽기 버퍼와 기존 프레임버퍼 및 패킹 상태 복원
        glPixelStorei(GL_PACK_ALIGNMENT, alignment);
        glPixelStorei(GL_PACK_ROW_LENGTH, rowLength);
        glPixelStorei(GL_PACK_SKIP_ROWS, skipRows);
        glPixelStorei(GL_PACK_SKIP_PIXELS, skipPixels);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, static_cast<GLuint>(packBuffer));
        glReadBuffer(static_cast<GLenum>(readBuffer));
        glBindFramebuffer(GL_READ_FRAMEBUFFER, static_cast<GLuint>(framebuffer));

        // 9. 24비트 BMP 파일 헤더 구성
        // OpenGL과 양수 높이 BMP는 모두 아래쪽 행부터 저장하므로 뒤집지 않는다.
        Array<uint8, 54> header{};
        header[0] = 'B'; header[1] = 'M';
        Put(header, 2, static_cast<uint32>(imageSize + header.size()), 4);
        Put(header, 10, 54, 4);
        Put(header, 14, 40, 4);
        Put(header, 18, static_cast<uint32>(width), 4);
        Put(header, 22, static_cast<uint32>(height), 4);
        Put(header, 26, 1, 2);
        Put(header, 28, 24, 2);
        Put(header, 34, static_cast<uint32>(imageSize), 4);

        // 10. 현재 실행 중인 exe의 전체 경로 조회
        List<wchar_t> executablePath(32768, L'\0');
        const DWORD length = GetModuleFileNameW(nullptr, executablePath.data(), static_cast<DWORD>(executablePath.size()));
        if (length == 0 || length >= executablePath.size())
            return Fail(error, "Failed to resolve the executable path.");

        // 11. UTF-8 입력에서 마지막 파일명만 추출하고 exe 폴더의 저장 경로 구성
        const ByteString utf8FileName(fileName.begin(), fileName.end());
        const Path name = Path(utf8FileName).filename();
        if (name.empty() || name == "." || name == ".." || name.has_root_path())
            return Fail(error, "The snapshot file name is invalid.");
        const Path outputPath = Path(executablePath.data()).parent_path() / name;

        // 12. 저장 파일을 바이너리 모드로 열기 (같은 이름의 기존 파일은 덮어쓰기)
        OutputFileStream file(outputPath, OutputFileStream::binary | OutputFileStream::trunc);
        if (!file) return Fail(error, "Failed to open the snapshot file.");

        // 13. BMP 헤더와 픽셀을 기록한 뒤 파일을 닫고 최종 저장 결과 확인
        file.write(reinterpret_cast<cstr>(header.data()), static_cast<StreamSize>(header.size()));
        file.write(reinterpret_cast<cstr>(pixels.data()), static_cast<StreamSize>(pixels.size()));
        file.close();
        if (!file) return Fail(error, "Failed to write the snapshot file.");
        return true;
    }
}
