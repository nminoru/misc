#! /bin/sh

sudo apt update
sudo apt -y install xfce4 xfce4-goodies xfce4-clipman xrdp ubuntu-desktop fonts-noto-cjk language-pack-ja fonts-takao
sudo apt -y install firefox curl wget gnupg ca-certificates
sudo localectl set-locale LANG=ja_JP.UTF-8
sudo systemctl enable xrdp

sudo apt -y install unattended-upgrades
sudo dpkg-reconfigure --priority=low unattended-upgrades

for arg in "$@"; do
    # Azure CLI & Azure Export for Terraform
    if [ $arg = "azure" ]; then
	if [[ ! -f "/etc/apt/keyrings/microsoft.gpg" ]]; then
	    wget -O - https://packages.microsoft.com/keys/microsoft.asc | sudo gpg --dearmor -o /etc/apt/keyrings/microsoft.gpg
	    sudo chmod go+r /etc/apt/keyrings/microsoft.gpg
	fi
	sudo apt-add-repository -y https://packages.microsoft.com/ubuntu/24.04/prod
	sudo apt -y install azure-cli aztfexport
    fi

    # Terraform
    if [ $arg = "terraform" ]; then
	wget -O - https://apt.releases.hashicorp.com/gpg | sudo gpg --dearmor -o /etc/apt/keyrings/hashicorp-archive-keyring.gpg
	sudo chmod go+r /etc/apt/keyrings/hashicorp-archive-keyring.gpg
	echo "deb [signed-by=/etc/apt/keyrings/hashicorp-archive-keyring.gpg] https://apt.releases.hashicorp.com $(lsb_release -cs) main" | sudo tee /etc/apt/sources.list.d/hashicorp.list
	sudo apt -y install terraform
    fi    
    
    # Googe Chrome    
    if [ $arg = "chrome" ]; then
	wget https://dl.google.com/linux/direct/google-chrome-stable_current_amd64.deb
	sudo apt -y install ./google-chrome-stable_current_amd64.deb
    fi

    # Visual Studio Code    
    if [ $arg = "code" ]; then
	if [[ ! -f "/etc/apt/keyrings/microsoft.gpg" ]]; then
	    wget -O - https://packages.microsoft.com/keys/microsoft.asc | sudo gpg --dearmor -o /etc/apt/keyrings/microsoft.gpg
	    sudo chmod go+r /etc/apt/keyrings/microsoft.gpg
	fi	
	echo "deb [arch=amd64 signed-by=/usr/share/keyrings/packages.microsoft.gpg] https://packages.microsoft.com/repos/code stable main" | sudo tee /etc/apt/sources.list.d/vscode.list
	sudo apt -y install code
    fi

    # DBeaver
    if [ $arg = "dbeaver" ]; then
	wget -O - https://dbeaver.io/debs/dbeaver.gpg.key | sudo gpg --dearmor -o /usr/share/keyrings/dbeaver.gpg
	echo "deb [signed-by=/usr/share/keyrings/dbeaver.gpg] https://dbeaver.io/debs/dbeaver-ce /" | sudo tee /etc/apt/sources.list.d/dbeaver.list
	sudo apt -y install dbeaver-ce
    fi
done

sudo apt update
sudo apt -y upgrade

