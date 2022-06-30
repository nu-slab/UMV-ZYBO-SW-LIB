import cv2
import glob

# 終了基準
criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)

# チェスボードで６×７の交点を検出しよう
objp = np.zeros((6*7,3), np.float32)
objp[:,:2] = np.mgrid[0:7,0:6].T.reshape(-1,2)

# 検出した交点を格納する配列
objpoints = [] # 3次元点
imgpoints = [] # 2次元点

images = glob.glob('../img/*.png')

for fname in images:
    img = cv2.imread(fname)
    gray = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)

    # チェスボードの交点を検出
    ret, corners = cv2.findChessboardCorners(gray, (7,6),None)

    # 交点が検出された場合
    if ret == True:
      # 成功したファイル名を出力
        print(fname)
        objpoints.append(objp)

        corners2 = cv2.cornerSubPix(gray,corners,(11,11),(-1,-1),criteria)
        imgpoints.append(corners2)

        # 交点を描画する
        img = cv2.drawChessboardCorners(img, (7,6), corners2,ret)
        cv2.imshow('img',img)
        cv2.waitKey(500)

cv2.destroyAllWindows()
