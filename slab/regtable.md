# スレーブのアドレスとターゲットの対応（2021 ver.）

## read (PS <- PL)

| アドレス | 名前                   | 説明                                             |
|     ---: | :---                   | :---                                             |
|     0x00 | wheel_feedback_r       | 右車輪フィードバック値                           |
|     0x01 | wheel_feedback_l       | 左車輪フィードバック値                           |
|     0x02 | lsdbuf_ready_f         | フロントカメラ線分格納バッファ書き込み可能フラグ |
|     0x03 | lsdbuf_ready_r         | リアカメラ線分格納バッファ書き込み可能フラグ     |
|     0x04 | lsdbuf_line_num_f      | フロントカメラ線分格納バッファの線分検出個数     |
|     0x05 | lsdbuf_line_num_r      | リアカメラ線分格納バッファの線分検出個数         |
|     0x06 | lsdbuf_start_h_f       | フロントカメラ線分格納バッファの線分始点のx座標  |
|     0x07 | lsdbuf_start_h_r       | リアカメラの線分格納バッファ線分始点のx座標      |
|     0x08 | lsdbuf_end_h_f         | フロントカメラ線分格納バッファ線分終点のx座標    |
|     0x09 | lsdbuf_end_h_r         | リアカメラ線分格納バッファ線分終点のx座標        |
|     0x0a | lsdbuf_start_v_f       | フロントカメラ線分格納バッファの線分始点のy座標  |
|     0x0b | lsdbuf_start_v_r       | リアカメラの線分格納バッファ線分始点のy座標      |
|     0x0c | lsdbuf_end_v_f         | フロントカメラ線分格納バッファ線分終点のy座標    |
|     0x0d | lsdbuf_end_v_r         | リアカメラ線分格納バッファ線分終点のy座標        |
|     0x0e | lsdbuf_angle_f         | フロントカメラ線分格納バッファ線分の勾配         |
|     0x0f | lsdbuf_angle_r         | リアカメラ線分格納バッファ線分の勾配             |
|     0x10 | N/A                    | N/A                                              |
|     0x11 | N/A                    | N/A                                              |
|     0x12 | N/A                    | N/A                                              |
|     0x13 | N/A                    | N/A                                              |
|     0x14 | N/A                    | N/A                                              |
|     0x15 | N/A                    | N/A                                              |
|     0x16 | N/A                    | N/A                                              |
|     0x17 | N/A                    | N/A                                              |
|     0x18 | N/A                    | N/A                                              |
|     0x19 | N/A                    | N/A                                              |
|     0x1a | N/A                    | N/A                                              |
|     0x1b | N/A                    | N/A                                              |
|     0x1c | N/A                    | N/A                                              |
|     0x1d | N/A                    | N/A                                              |
|     0x1e | N/A                    | N/A                                              |
|     0x1f | sw                     | デバッグ用                                       |

## write (PS -> PL)

| アドレス | 名前                   | 説明                                               |
|     ---: | :---                   | :---                                               |
|     0x00 | wheel_accel_r          | 右アクセル値                                       |
|     0x01 | wheel_accel_l          | 左アクセル値                                       |
|     0x02 | wheel_brake            | 緊急ブレーキ                                       |
|     0x03 | wheel_pid_kp_r         | 右車輪用PID制御の比例ゲイン                        |
|     0x04 | wheel_pid_kp_l         | 左車輪用PID制御の比例ゲイン                        |
|     0x05 | wheel_pid_ki_r         | 右車輪用PID制御の積分ゲイン                        |
|     0x06 | wheel_pid_ki_l         | 左車輪用PID制御の積分ゲイン                        |
|     0x07 | wheel_pid_kd_r         | 右車輪用PID制御の微分ゲイン                        |
|     0x08 | wheel_pid_kd_l         | 左車輪用PID制御の微分ゲイン                        |
|     0x09 | wheel_pid_bias_r       | 右車輪用PID制御のバイアス                          |
|     0x0a | wheel_pid_bias_l       | 左車輪用PID制御のバイアス                          |
|     0x0b | lsd_grad_thres         | LSDの勾配閾値     			         |
|     0x0c | lsdbuf_raddr_f         | フロントカメラの線分格納バッファ読み出しアドレス   |
|     0x0d | lsdbuf_raddr_r         | リアカメラの線分格納バッファ読み出しアドレス       |
|     0x0e | lsdbuf_write_protect_f | フロントカメラの線分格納バッファ書き込み禁止フラグ |
|     0x0f | lsdbuf_write_protect_r | リアカメラの線分格納バッファ書き込み禁止フラグ     |
|     0x10 | N/A                    | N/A                                                |
|     0x11 | N/A                    | N/A                                                |
|     0x12 | N/A                    | N/A                                                |
|     0x13 | N/A                    | N/A                                                |
|     0x14 | N/A                    | N/A                                                |
|     0x15 | N/A                    | N/A                                                |
|     0x16 | N/A                    | N/A                                                |
|     0x17 | N/A                    | N/A                                                |
|     0x18 | N/A                    | N/A                                                |
|     0x19 | N/A                    | N/A                                                |
|     0x1a | N/A                    | N/A                                                |
|     0x1b | N/A                    | N/A                                                |
|     0x1c | N/A                    | N/A                                                |
|     0x1d | N/A                    | N/A                                                |
|     0x1e | N/A                    | N/A                                                |
|     0x1f | led                    | デバッグ用                                         |
