1.

# 1. python3-full がインストールされていることを確認
sudo apt install python3-full

# 2. 仮想環境を作成
python3 -m venv ~/myenv

# 3. 仮想環境を有効化
source ~/myenv/bin/activate

# 4. pip3 install を実行
pip install pyvmomi paramiko scp


2.

export ESXI_IPADDR=
export ESXI_USER=
export ESXI_PASSWORD=
