
import gc
import horde3d as h3d
from PythonQt import QtGui
from PiwMainWindow import scene_tree_widget, scene_tree_widget_default_model, node_property_widget, horde_window

gModel = scene_tree_widget_default_model

def construct_tree(root) :
    # get a plain list of current scene
    cnt = h3d.findNodes(h3d.RootNode, "", h3d.NodeTypes.Undefined)
    if cnt == 0 : return
    
    local_dict = {}
    for i in range(cnt) :
        node_id = h3d.getNodeFindResult(i)
        if node_id == h3d.RootNode : 
            continue
        node_name = h3d.getNodeParamStr(node_id, h3d.NodeParams.NameStr)
        # sikp the editor's interal nodes
        if node_name.startswith('__') and node_name.endswith('__') : 
            continue
        local_dict[node_id] = gModel.createItem(node_name, node_id)

    # add children recursively
    for nid, item in local_dict.iteritems() :
        pid = h3d.getNodeParent(nid)
        try :
            parent = gModel.getItem(local_dict[pid])
            parent.appendRow(gModel.getItem(item))
        except KeyError :
            # it's top level node, or its parent is a editor node
            root.appendRow(gModel.getItem(item))
    
horde_connect = get_connector(horde_window)

proto_node = QtGui.QStandardItem("Node")

@horde_connect('sceneLoaded()')
def setup_scene_tree_widget():
    scene_tree_widget.setModel(gModel)
    gModel.clearItems()
    gModel.setHorizontalHeaderLabels(['Scene Nodes'])
    root = gModel.invisibleRootItem()
    construct_tree(root)
    
@connect('clicked(QModelIndex)')
def on_activate(idx):
    model = scene_tree_widget.model()
    item = model.itemFromIndex(idx)
    if item: node_property_widget.addH3DObject(item.data())


def on_changed(item):
    # remove or change its name
    if not item.data() :
        gModel.removeRow(item.row(), item.index().parent())
    else :
        # change node name or its parent
        node_id = item.data()
        pid = h3d.getNodeParent(node_id)
        node_name = h3d.getNodeParamStr(node_id, h3d.NodeParams.NameStr)
        # change parent
        if not item.parent() and pid != h3d.RootNode :
            _, mat_abs = h3d.getNodeTransMats(node_id)
            h3d.setNodeParent(node_id, h3d.RootNode)
            h3d.setNodeTransMat(node_id, mat_abs)
        elif item.parent().data() != pid :
            new_pid = item.parent().data()
            _, pmat_abs = h3d.getNodeTransMats(new_pid)
            _, mat_abs = h3d.getNodeTransMats(node_id)
            h3d.setNodeParent(node_id, new_pid)
            h3d.setNodeTransMat(node_id, (pmat_abs**-1)*mat_abs)
        # change parent
        if node_name != item.text() :
            h3d.setNodeParamStr(node_id, h3d.NodeParams.NameStr, item.text())
        on_activate(item.index())


gModel.connect('itemChanged(QStandardItem*)', on_changed)

