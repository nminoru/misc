from pyVim.connect import SmartConnect, Disconnect
from pyVmomi import vim
from pyVim.task import WaitForTask
import ssl
import os

esxi_ipaddr = os.environ['ESXI_IPADDR']
esxi_user = os.environ['ESXI_USER']
esxi_password = os.environ['ESXI_PASSWORD']


# SSL証明書の警告を無視
context = ssl._create_unverified_context()

# ESXiまたはvCenterに接続
si = SmartConnect(
    host=esxi_ipaddr,
    user=esxi_user,
    pwd=esxi_password,
    sslContext=context
)

for datacenter in content.rootFolder.childEntity:
    for vm_obj in datacenter.vmFolder.childEntity:
        print(vm_obj.name + " : " + vm_obj._moId)
        
Disconnect(si)
