# GBA（ゲームボーイアドバンス）のエミュレータ上で動くリズムゲームシミュレータの作成
大学の講義にて，GBAエミュレータを用いた組込みプログラミングの学習を行った．  
講義のグループワークの一環でシューティングゲームを作成したが，リズムゲームの方が面白そうなので独自にリズムゲームを作成した． 
(なお，Hello Git)  

## 動作環境
windows10以上

## 遊び方
1. binフォルダのvbawin.exeを起動．（GBAエミュレータ本体）
2. エミュレータでrhythm_game_simulator.mbを読み込む

## コンパイルについて（Makefile）
Cファイル変更後，

make NAME=rhythm_game_simulator

コマンドを実行．mbファイルが出来ればコンパイル成功


### rhythm_game_simulator
Created a simple game on a GBA (GameBoy Advance) emulator.
