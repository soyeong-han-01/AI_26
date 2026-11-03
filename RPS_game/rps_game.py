import cv2
from ultralytics import YOLO
import time

# 1. TensorRT 엔진 로드
model = YOLO("my_rps_yolo11n.engine", task="detect")

# 클래스 ID와 텍스트 매핑 딕셔너리 정의
ansToText = {0: 'scissors', 1: 'rock', 2: 'paper'}

# 카메라 설정 (웹캠)
cap = cv2.VideoCapture(0)
# 엔진 크기에 맞춰 카메라 입력 해상도를 320x240으로 고정합니다.
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 320)  
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 240)
cap.set(cv2.CAP_PROP_BUFFERSIZE, 1)

cv2.namedWindow("cam", cv2.WINDOW_NORMAL)
cv2.resizeWindow("cam", 640, 480)

# 가위바위보 승패 판정 함수
def judge_rps(left_hand, right_hand):
    if left_hand == right_hand:
        return "DRAW"
    elif (left_hand == 'rock' and right_hand == 'scissors') or \
         (left_hand == 'scissors' and right_hand == 'paper') or \
         (left_hand == 'paper' and right_hand == 'rock'):
        return "LEFT WIN"
    else:
        return "RIGHT WIN"

prev_time = time.time()
fps = 0.0

while cap.isOpened():
    success, frame = cap.read()
    if not success:
        break

    # 사람이 볼 때 직관적이도록 좌우 반전 (거울 모드)
    frame = cv2.flip(frame, 1)

    # 2. 추론 실행 
    # [수정] imgsz를 엔진 규격에 맞게 반드시 320으로 설정해야 에러가 나지 않습니다.
    # conf를 0.4으로 낮춰 320 해상도에서도 손이 최대한 잘 잡히도록 유도합니다.
    results = model(frame, imgsz=320, conf=0.4, iou=0.45, device=0, max_det=2, verbose=False)[0]
    
    # TensorRT 에러 방지: 결과 객체의 클래스 이름 맵을 강제로 변경합니다.
    results.names = ansToText
    
    # 기본 바운딩 박스 시각화
    frame = results.plot()

    # 3. 양손 검출 및 승패 판정 로직
    boxes = results.boxes
    if len(boxes) == 2:
        detected_hands = []
        
        for box in boxes:
            cls_id = int(box.cls[0])
            label = ansToText[cls_id]
            
            # 바운딩 박스 좌표에서 x1, x2를 추출하여 중심 X 좌표 계산 (PyTorch 텐서 슬라이싱 안전화)
            xyxy = box.xyxy[0].tolist()  # [x1, y1, x2, y2]
            x1, x2 = xyxy[0], xyxy[2]
            x_center = (x1 + x2) / 2
            
            detected_hands.append({'label': label, 'x': x_center})
        
        # X 좌표 기준으로 정렬 (화면 기준 왼쪽 손이 0번, 오른쪽 손이 1번)
        detected_hands = sorted(detected_hands, key=lambda item: item['x'])
        
        left_hand = detected_hands[0]['label']
        right_hand = detected_hands[1]['label']
        
        # 승패 판정
        result_text = judge_rps(left_hand, right_hand)
        
        # 화면 상단에 결과 출력 (640x480 해상도에 맞게 좌표 및 글씨 크기 조정)
        cv2.rectangle(frame, (20, 10), (300, 70), (0, 0, 0), -1) # 검은색 배경 박스
        cv2.putText(frame, f"L: {left_hand.upper()} vs R: {right_hand.upper()}", (30, 35),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1, lineType=cv2.LINE_AA)
        cv2.putText(frame, f"RESULT: {result_text}", (80, 60),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 255), 2, lineType=cv2.LINE_AA)
    else:
        # 손이 2개가 아닐 때 안내 메시지
        cv2.putText(frame, f"Show both hands! ({len(boxes)}/2)", (70, 30),
                    cv2.FONT_HERSHEY_DUPLEX, 0.5, (0, 0, 255), 1, lineType=cv2.LINE_AA)

    # FPS 계산 및 출력
    cur_time = time.time()
    dt = cur_time - prev_time
    if dt > 0: 
        fps = 1.0 / dt
    prev_time = cur_time

    cv2.putText(frame, f"FPS: {fps:.1f}", (10, 220), cv2.FONT_HERSHEY_SIMPLEX,
                0.5, (0, 255, 0), 1, lineType=cv2.LINE_AA)

    # 화면 표시용 영상만 확대
    display_frame = cv2.resize(
    frame,
    (640, 480),
    interpolation=cv2.INTER_LINEAR
    )
    cv2.imshow("cam", display_frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
