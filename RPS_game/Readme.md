# 가위바위보 게임 실행 방법

## 1. 프로젝트 소개

웹캠으로 두 손의 가위·바위·보 동작을 인식하고, 화면에서 왼쪽 손과 오른쪽 손의 승패를 판정하는 프로그램입니다.

- 객체 검출 모델: YOLO11n
- 학습 클래스: `scissors`, `rock`, `paper`
- 추론 형식: TensorRT Engine
- 실행 장치: Jetson Orin Nano
- 카메라 입력 해상도: 320 × 240
- 모델 입력 크기: 320
- 화면 출력 크기: 640 × 480

## 2. 주요 파일

```text
rps_game.py                # 가위바위보 게임 실행 코드
my_rps_yolo11n.onnx        # 직접 학습한 ONNX 모델
my_rps_yolo11n.engine      # TensorRT 변환 모델
실행방법.md                 # 실행 방법 설명
```

`my_rps_yolo11n.engine` 파일이 없다면 아래 4번 과정을 통해 ONNX 파일을 TensorRT 엔진으로 변환합니다.

## 3. 실행 환경

- Python 가상환경: `venv`
- OpenCV
- Ultralytics
- TensorRT 10.3.0
- USB 웹캠: `/dev/video0`

GUI 화면을 원격으로 확인하는 경우 XLaunch를 먼저 실행하고 Jetson에 접속합니다.

## 4. ONNX 모델을 TensorRT 엔진으로 변환

프로젝트 폴더로 이동합니다.

```bash
cd ~/work/examples/04_Object_Detection_Based_On-Device_AI
```

다음 명령으로 ONNX 모델을 TensorRT 엔진으로 변환합니다.

```bash
trtexec --onnx=my_rps_yolo11n.onnx --saveEngine=my_rps_yolo11n.engine
```

변환이 완료되면 다음 명령으로 파일 생성을 확인합니다.

```bash
ls -lh my_rps_yolo11n.engine
```

## 5. 가위바위보 게임 실행

프로젝트 폴더에서 가상환경을 활성화합니다.

```bash
source venv/bin/activate
```

프로그램을 실행합니다.

```bash
python rps_game.py
```

프로그램에서 사용하는 모델은 다음과 같이 지정되어 있습니다.

```python
model = YOLO("my_rps_yolo11n.engine", task="detect")
```

## 6. 게임 방법

1. 카메라 화면에 두 손이 모두 보이도록 합니다.
2. 각 손으로 가위, 바위 또는 보 동작을 만듭니다.
3. 프로그램이 화면상의 위치를 기준으로 왼쪽 손과 오른쪽 손을 구분합니다.
4. 두 손이 인식되면 화면 상단에 각 손의 동작과 판정 결과가 표시됩니다.

판정 결과는 다음과 같습니다.

| 출력 문구 | 의미 |
|---|---|
| `LEFT WIN` | 화면의 왼쪽 손 승리 |
| `RIGHT WIN` | 화면의 오른쪽 손 승리 |
| `DRAW` | 무승부 |
| `Show both hands!` | 두 손이 모두 인식되지 않음 |

프로그램을 종료하려면 카메라 창을 선택한 상태에서 `q` 키를 누릅니다.

## 7. 주요 설정

학습 클래스 번호는 다음과 같습니다.

```python
ansToText = {
    0: "scissors",
    1: "rock",
    2: "paper"
}
```

카메라 영상은 사용자가 보기 편하도록 좌우 반전된 거울 모드로 표시합니다.

```python
frame = cv2.flip(frame, 1)
```

모델 추론 설정은 다음과 같습니다.

```python
results = model(
    frame,
    imgsz=320,
    conf=0.4,
    iou=0.45,
    device=0,
    max_det=2,
    verbose=False
)[0]
```

화면에는 추론 결과, 왼쪽·오른쪽 손의 판정 결과와 FPS가 표시됩니다.

## 8. 실행 시 참고 사항

- 웹캠이 연결되어 있어야 합니다.
- 카메라가 열리지 않으면 `ls /dev/video*` 명령으로 `/dev/video0`을 확인합니다.
- XLaunch 창이 표시되지 않으면 XLaunch와 원격 접속을 다시 시작합니다.
- TensorRT 엔진은 사용하는 Jetson 장치에서 생성하는 것을 권장합니다.
- 프로그램이 엔진 로딩 후 멈춘 경우 실행 중인 프로그램을 종료한 뒤 Jetson을 재부팅하고 다시 실행합니다.
