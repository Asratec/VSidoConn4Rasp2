# VSidoConn4Rasp2  
V-Sido CONNECT Sample Code For Raspbery pi 2 Board  
  

Raspbery pi 2 Boardの準備.
  動作確認済みファームウェア  
   http://director.downloads.raspberrypi.org/raspbian/images/raspbian-2015-02-17/2015-02-16-raspbian-wheezy.zip  


	
ビルド済みバイナリをインストールする。  
  https://v-sido-developer.com/learning/connect/v-sido-connect-raspberrypi/  

	
ソースコードをビルドする.  
1.コマンドラインから、ビルド必要環境をインストールする。  
   wget --no-check-certificate -O - https://asratec.github.io/VSidoConn4Rasp2/buildenv.sh | sudo sh  
2.ソースコードをチェックアウトする  
  git clone https://github.com/Asratec/VSidoConn4Rasp2.git  
3.依存ソースコードをチェックアウトする  
  cd VSidoConn4Rasp2  
  git clone https://github.com/Asratec/VSidoConnServer.git  
4.パッケージをビルドする。  
  make package    
  成果物はVSidoConn4Rasp2.tar.gzとなる。

成果物を実行環境にインストールする  
  sudo mkdir -p /opt/vsido/  
  sudo tar -xzvf VSidoConn4Rasp2.tar.gz -C /opt/vsido/  
  sudo make -C /opt/vsido/usr/share/Config  
  sync  
  sync  

  
