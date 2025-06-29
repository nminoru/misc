from pyVim.connect import SmartConnect, Disconnect
from pyVmomi import vim
from pyVim.task import WaitForTask
from scp import SCPClient
import paramiko
import ssl
import os

esxi_ipaddr = os.environ['ESXI_IPADDR']
esxi_user = os.environ['ESXI_USER']
esxi_password = os.environ['ESXI_PASSWORD']

target_snapshot_name = "for_backup_snapshot"
config_file = 'vmbackup.cfg'
backup_dir = 'vmbackup-folder'

vm_names = []


#
#
#
def backup_vm(target_vm_name):

    print("backup_vm: " + target_vm_name)

    vm = None
    
    for datacenter in content.rootFolder.childEntity:
        for vm_obj in datacenter.vmFolder.childEntity:
            if vm_obj.name == target_vm_name:
                vm = vm_obj
                break
            # if vm_obj._moId == target_vm_id:
            #     vm = vm_obj
            #     break
            
    # スナップショットの作成
    if vm:
        task = vm.CreateSnapshot_Task(
            name=target_snapshot_name,
            description=target_snapshot_name,
            memory=False,      # VMのメモリ状態を含めない
            quiesce=False      # ファイルシステムのクワイエスを行わない
        )
        print("スナップショット作成中...")    

        # タスク完了を待つ
        WaitForTask(task)

        if task.info.state == "success":
            print("スナップショット作成完了！")
        else:
            print("スナップショット作成に失敗:", task.info.error)
            return

        # バックアップ元
        raw_path = vm_obj.config.files.vmPathName  # 例: "[datastore1] VMName/VMName.vmx" 
        datastore_name = raw_path.split(']')[0].strip('[') 
        vm_subpath = raw_path.split(']')[1].strip().rsplit('/', 1)[0] 
        from_full_path = f"/vmfs/volumes/{datastore_name}/{vm_subpath}/"

        # バックアップ先
        to_full_path = f"{backup_dir}/{target_vm_name}/"

        # SSH接続
        ssh = paramiko.SSHClient()
        ssh.load_system_host_keys()
        ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        ssh.connect(esxi_ipaddr, port=22, username=esxi_user, password=esxi_password)

        # SCP転送
        with SCPClient(ssh.get_transport()) as scp:
            scp.put(to_full_path, recursive=True, remote_path=from_full_path)

        # スナップショットを削除する    
        snap_obj = find_snapshot_by_name(vm.snapshot.rootSnapshotList, target_snapshot_name)
        if snap_obj:
            print(f"スナップショット '{target_snapshot_name}' を削除中...")
            task = snap_obj.RemoveSnapshot_Task(removeChildren=False)
            WaitForTask(task)
            if task.info.state == "success":
                print("スナップショットを削除しました。")
            else:
                print("削除に失敗しました:", task.info.error)
        else:
            print("指定されたスナップショットが見つかりません。")
    else:
        print("対象のVMが見つかりません。")



# 設定ファイルの読みこみ
with open(config_file, 'r') as f:
    for line in f:
        if not line.strip().startswith("#"):
            vm_name = line.strip()
            if vm_name:
                vm_names.append(vm_name)

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

for vm_name in vm_names:
    backup_vm(vm_name)

Disconnect(si)
        
