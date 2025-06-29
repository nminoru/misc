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

content = si.RetrieveContent()

for datacenter in content.rootFolder.childEntity:
    for vm_obj in datacenter.vmFolder.childEntity:
        # print(vm_obj._moId + ',' + vm_obj.name)
        
        raw_path = vm_obj.config.files.vmPathName  # 例: "[datastore1] VMName/VMName.vmx" 
        datastore_name = raw_path.split(']')[0].strip('[') 
        vm_subpath = raw_path.split(']')[1].strip().rsplit('/', 1)[0] 
        full_path = f"/vmfs/volumes/{datastore_name}/{vm_subpath}/"

        print(vm_obj._moId + ',' + vm_obj.name + "," + full_path)
        
Disconnect(si)
