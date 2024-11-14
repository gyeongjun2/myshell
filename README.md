# Linux Shell 프로그램

## 프로젝트 개요

- 리눅스 시스템에서 동작하는 간단한 셸을 구현하여 명령어를 실행하고 여러 인자를 처리하며 시그널 및 에러 핸들링을 포함한 기본적인 쉘 기능을 제공
- 사용자 입력을 통해 명령어를 분석하고 필요한 경우 외부 프로그램을 실행할 수 있는 쉘 환경을 제공

## 프로젝트 목적

- 리눅스 환경에서의 쉘 동작 원리를 이해하고 시스템 프로그래밍과 시그널, 프로세스 생성&관리 등을 직접 구현하고 이를 통해 리눅스 쉘의 기본적인 작동 원리와 시스템 콜, 프로세스 간 통신을 파악

## 개발 인원 / 개발 기간

개발 인원 : 박경준

개발 기간 : 2024.05.17 ~ 2024.05.31

## 기술 스택

주요 언어 : C언어

개발 환경 : liunx

## 핵심 기능

**기본 리눅스 쉘 기능**

<img src="https://github.com/user-attachments/assets/c82a8146-8a6e-40c2-ba33-23e53bece185" width="400" height="300"/>

- 사용자가 입력한 명령어를 분석하고 실행하여 결과를 반환하는 기본적인 쉘 역할 수행

**다중 인자 처리**

<img src="https://github.com/user-attachments/assets/e5a8e4b8-bef9-464c-8524-cc51bc40e9c5" width="400" height="300"/>

- ls -a와 같은 여러 인자가 있는 명령어를 처리할 수 있도록 구현

**특정 프로그램 컴파일 & 실행**

<img src="https://github.com/user-attachments/assets/2bcd3ddc-babb-4afc-8af2-e43aafb1ff82" width="300" height="400"/>

- gcc와 같은 컴파일러 명령어, ./a.exe와 같은 실행 파일 명령어를 인식하고 처리

**시그널 처리**

<img src="https://github.com/user-attachments/assets/fbc82f16-e9ed-40ce-a5ff-ffaa9351d35e" width="300" height="100"/>
<img src="https://github.com/user-attachments/assets/9146f294-3d35-43e6-9ac4-73b439e04557" width="300" height="100"/>
<img src="https://github.com/user-attachments/assets/aabf5528-93d4-4494-8741-411ff1cd24bc" width="300" height="100"/>

- SIGINT 시그널을 무시하여 쉘이 중단되지 않도록 처리

**에러 핸들링**

- 명령어 실행 실패시 사용자에게 적절한 에러 메세지 출력