# 3D Guitar Amp Renderer (for AES Design Competition)

## 目的

音場再現技術を用いて没入感のあるギター演奏体験を作る

## 方法

[このサーベイ](https://github.com/tmtakashi/spherical-array-binaural-survey)に書かれている理論をベースにレンダリングエンジンを作る

1. まずはオフラインで方向別ステレオIRを生成できるようにする．
2. JUCEを導入し，畳み込み&GUIで回転できるプラグインを作る．
3. （野望）センサーと[OSC](https://docs.juce.com/master/tutorial_osc_sender_receiver.html)を使ってヘッドトラッキング機能をつける．