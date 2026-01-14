#! /bin/sh

sudo apt update
sudo apt -y install xfce4 xfce4-goodies xfce4-clipman xrdp ubuntu-desktop fonts-noto-cjk language-pack-ja fonts-takao
sudo apt -y install firefox curl wget gnupg ca-certificates
sudo localectl set-locale LANG=ja_JP.UTF-8
sudo systemctl enable xrdp

sudo apt -y install unattended-upgrades
sudo dpkg-reconfigure --priority=low unattended-upgrades

for arg in "$@"; do
    # Googe Chrome    
    if [ $arg = "chrome" ]; then

	wget https://dl.google.com/linux/direct/google-chrome-stable_current_amd64.deb
	sudo apt -y install ./google-chrome-stable_current_amd64.deb
    fi

    # Visual Studio Code    
    if [ $arg = "code" ]; then
	wget -qO- https://packages.microsoft.com/keys/microsoft.asc | gpg --dearmor > packages.microsoft.gpg
	sudo install -o root -g root -m 644 packages.microsoft.gpg /usr/share/keyrings/
	echo "deb [arch=amd64 signed-by=/usr/share/keyrings/packages.microsoft.gpg] https://packages.microsoft.com/repos/code stable main" | sudo tee /etc/apt/sources.list.d/vscode.list
	sudo apt -y install code
    fi

    # DBeaver
    if [ $arg = "dbeaver" ]; then
	wget -O - https://dbeaver.io/debs/dbeaver.gpg.key | gpg --dearmor | sudo tee /usr/share/keyrings/dbeaver.gpg > /dev/null
	echo "deb [signed-by=/usr/share/keyrings/dbeaver.gpg] https://dbeaver.io/debs/dbeaver-ce /" | sudo tee /etc/apt/sources.list.d/dbeaver.list
	sudo apt -y install dbeaver-ce
    fi
done

sudo apt update
sudo apt -y upgrade

