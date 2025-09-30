# 참고한 환경 설정
https://grr1.tistory.com/23
https://blog.naver.com/jdkim2004/223211503446
일부 lib파일은 변경 필요.

# release 버전 빌드
visual studio x64 native tools command prompt에서
```bash
cd C:\wxWidgets-3.2.8\build\msw
nmake /f makefile.vc BUILD=release SHARED=0 MONOLITHIC=0 TARGET_CPU=amd64
```

# 테스트 용 호스트 설정
```bash
git fetch origin
git reset --hard origin/main
git clean -fdx
```