/*!
 * Tine 2.0 - Filemanager 
 * Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 * http://www.gnu.org/licenses/agpl.html AGPL Version 3
 */
/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Martin Jatho <m.jatho@metaways.de>
 * @copyright   Copyright (c) 2007-2012 Metaways Infosystems GmbH (http://www.metaways.de)
 */
Ext.ns('Tine.Filemanager');

    
Tine.Filemanager.GridContextMenu = {
    /**
     * create tree node
     */
    addNode: function() {
        Tine.log.debug("grid add");
    },
    
    /**
     * rename tree node
     */
    renameNode: function() {
        if (this.scope.ctxNode) {
            
            var node = this.scope.ctxNode[0];
            
            var nodeText = node.data.name;
            if(typeof nodeText == 'object') {
                nodeText = nodeText.name;
            }
            
            Ext.MessageBox.show({
                title: 'Rename ' + this.nodeName,
                msg: String.format(_('Please enter the new name of the {0}:'), this.nodeName),
                buttons: Ext.MessageBox.OKCANCEL,
                value: nodeText,
                fn: function(_btn, _text){
                    if (_btn == 'ok') {
                        if (! _text) {
                            Ext.Msg.alert(String.format(_('Not renamed {0}'), this.nodeName), String.format(_('You have to supply a {0} name!'), this.nodeName));
                            return;
                        }
                        
                        var params = {
                                method: this.backend + '.rename' + this.backendModel,
                                newName: _text
                        };
                        
                        if (this.backendModel == 'Node') {
                            params.application = this.scope.app.appName || this.scope.appName;
                            var filename = node.data.path;
                            params.sourceFilenames = [filename];
                            
                            var targetFilename = "/";
                            var sourceSplitArray = filename.split("/");
                            for (var i=1; i<sourceSplitArray.length-1; i++) {
                                targetFilename += sourceSplitArray[i] + '/';
                            }
                            
                            params.destinationFilenames = [targetFilename + _text];
                            params.method = this.backend + '.moveNodes';
                        }
                        
                        Ext.Ajax.request({
                            params: params,
                            scope: this,
                            success: function(_result, _request){
                                var nodeData = Ext.util.JSON.decode(_result.responseText)[0];
                                this.scope.fireEvent('containerrename', nodeData);
                                
                                // TODO: im event auswerten
                                if (this.backendModel == 'Node') {
                                    var grid = this.scope.app.getMainScreen().getCenterPanel();
                                    grid.getStore().reload();
                                    
                                    var nodeName = nodeData.name;
                                    if(typeof nodeName == 'object') {
                                        nodeName = nodeName.name;
                                    }
                                    
                                    var treeNode = this.scope.app.getMainScreen().getWestPanel().getContainerTreePanel().getNodeById(nodeData.id);
                                    if(treeNode) {
                                        treeNode.setText(nodeName);
                                        treeNode.attributes.nodeRecord.beginEdit();
                                        treeNode.attributes.nodeRecord.set('name', nodeName); // TODO set path
                                        treeNode.attributes.nodeRecord.set('path', nodeData.path); // TODO set path
                                        treeNode.attributes.path = nodeData.path; // TODO set path
                                        treeNode.attributes.nodeRecord.commit(false);
                                        
                                        if(typeof treeNode.attributes.name == 'object') {
                                            treeNode.attributes.name.name = nodeName; // TODO set path
                                        }
                                        else {
                                            treeNode.attributes.name = nodeName;
                                        }
                                    }
                                }
                            },
                            failure: function(result, request) {
                                var nodeData = Ext.util.JSON.decode(result.responseText);
                                
                                var appContext = Tine[this.scope.app.appName];
                                if(appContext && appContext.handleRequestException) {
                                    appContext.handleRequestException(nodeData.data);
                                }
                            }
                        });
                    }
                },
                scope: this,
                prompt: true,
                icon: Ext.MessageBox.QUESTION
            });
        }
    },
    
    /**
     * delete tree node
     */
    deleteNode: function() {
        if (this.scope.ctxNode) {
            var nodes = this.scope.ctxNode;
            
            var nodeName = "";
            if(nodes && nodes.length) {
                for(var i=0; i<nodes.length; i++) {
                    var currNodeData = nodes[i].data;
                    
                    if(typeof currNodeData.name == 'object') {
                        nodeName += currNodeData.name.name + '<br />';
                    }
                    else {
                        nodeName += currNodeData.name + '<br />';
                    }
                }
                
            }
            
            this.conflictConfirmWin = Tine.widgets.dialog.FileListDialog.openWindow({
                modal: true,
                allowCancel: false,
                height: 180,
                width: 300,
                title: this.scope.app.i18n._('Do you really want to delete the following files?'),
                text: nodeName,
                scope: this,
                handler: function(button){
                    if (button == 'yes') {
                        var params = {
                                method: this.backend + '.delete' + this.backendModel
                        };
                        
                        if (this.backendModel == 'Node') {
                            
                            var filenames = new Array();
                            if(nodes) {
                                for(var i=0; i<nodes.length; i++) {
                                    filenames.push(nodes[i].data.path);
                                }
                            }
                            params.application = this.scope.app.appName || this.scope.appName;
                            params.filenames = filenames;
                            params.method = this.backend + ".deleteNodes";
                        }
                        
                        Ext.Ajax.request({
                            params: params,
                            scope: this,
                            success: function(_result, _request){
                                
                                if(nodes &&  this.backendModel == 'Node') {
                                    var treePanel = this.scope.app.getMainScreen().getWestPanel().getContainerTreePanel();
                                    for(var i=0; i<nodes.length; i++){
                                        treePanel.fireEvent('containerdelete', nodes[i].data.container_id);
                                        // TODO: in EventHandler auslagern
                                        var treeNode = treePanel.getNodeById(nodes[i].id);
                                        if(treeNode) {
                                            treeNode.parentNode.removeChild(treeNode);
                                        }
                                    }
                                    for(var i=0; i<nodes.length; i++) {
                                        var node = nodes[i];
                                        if(node.fileRecord) {
                                            var upload = Tine.Tinebase.uploadManager.getUpload(node.fileRecord.get('uploadKey'));
                                            upload.setPaused(true);
                                            Tine.Tinebase.uploadManager.unregisterUpload(upload.id);
                                        }
                                    }
                                    this.scope.app.getMainScreen().getCenterPanel().getStore().remove(nodes);
                                }
                            },
                            failure: function(result, request) {
                                var nodeData = Ext.util.JSON.decode(result.responseText);
                                
                                var appContext = Tine[this.scope.app.appName];
                                if(appContext && appContext.handleRequestException) {
                                    appContext.handleRequestException(nodeData.data);
                                }
                            }
                        });
                    }
    
                }
            });

        }
    },
    
    /**
     * change tree node color
     */
    changeNodeColor: function(cp, color) {
        Tine.log.debug("grid change color");
        
        
    },
    
    /**
     * manage permissions
     * 
     */
    managePermissions: function() {
        Tine.log.debug("grid manage permissions");
    },
    
    /**
     * reload node
     */
    reloadNode: function() {
        Tine.log.debug("grid reload node");
    },
    
    /**
     * calls the file edit dialog from the grid
     * @param {} button
     * @param {} event
     */
    onEditFile: function(button, event) {
        var app = Tine.Tinebase.appMgr.get('Filemanager');
        var grid = app.getMainScreen().getCenterPanel();
        grid.onEditFile.call(grid);
    },
    
    /**
     * download file
     * 
     * @param {} button
     * @param {} event
     */
    downloadFile: function(button, event) {
        
        var grid = this.scope.app.getMainScreen().getCenterPanel();
        var selectedRows = grid.selectionModel.getSelections();
        
        var fileRow = selectedRows[0];
               
        var downloadPath = fileRow.data.path;
        var downloader = new Ext.ux.file.Download({
            params: {
                method: 'Filemanager.downloadFile',
                requestType: 'HTTP',
                id: '',
                path: downloadPath
            }
        }).start();
    },
    
    /**
     * is the download context menu option visible / enabled
     * 
     * @param action
     * @param grants
     * @param records
     */
    isDownloadEnabled: function(action, grants, records) {
        for(var i=0; i<records.length; i++) {
            if(records[i].data.type === 'folder') {
                action.hide();
                return;
            }
        }
        action.show();
        
        var grid = this.scope.app.getMainScreen().getCenterPanel();
        var selectedRows = grid.selectionModel.getSelections();
        
        if(selectedRows.length > 1) {
            action.setDisabled(true);
        }
        else {
            action.setDisabled(false);
        }
        
    },
    
    /**
     * on pause
     * @param {} button
     * @param {} event
     */
    onPause: function (button, event) {

        var grid = this.scope;
        var gridStore = grid.store;
        gridStore.suspendEvents();
        var selectedRows = grid.selectionModel.getSelections();
        for(var i=0; i < selectedRows.length; i++) {
            var fileRecord = selectedRows[i];
            if(fileRecord.fileRecord) {
                fileRecord = fileRecord.fileRecord;
            }
            var upload = Tine.Tinebase.uploadManager.getUpload(fileRecord.get('uploadKey'));
            upload.setPaused(true);
        }
        gridStore.resumeEvents();
        grid.actionUpdater.updateActions(gridStore);
        this.scope.selectionModel.deselectRange(0, this.scope.selectionModel.getCount());
    },

    
    /**
     * on resume
     * @param {} button
     * @param {} event
     */
    onResume: function (button, event) {
        
        var grid = this.scope;
        var gridStore = grid.store;
        gridStore.suspendEvents();
        var selectedRows = grid.selectionModel.getSelections();
        for(var i=0; i < selectedRows.length; i++) {
            var fileRecord = selectedRows[i];
            if(fileRecord.fileRecord) {
                fileRecord = fileRecord.fileRecord;
            }
            var upload = Tine.Tinebase.uploadManager.getUpload(fileRecord.get('uploadKey'));
            upload.resumeUpload();
        }
        gridStore.resumeEvents();
        grid.actionUpdater.updateActions(gridStore);
        this.scope.selectionModel.deselectRange(0, this.scope.selectionModel.getCount());

    },
    
    /**
     * checks whether resume button shuold be enabled or disabled
     * 
     * @param action
     * @param grants
     * @param records
     */
    isResumeEnabled: function(action, grants, records) {
        
        for(var i=0; i<records.length; i++) {
            
            var record = records[i];
            if(record.fileRecord) {
                record = record.fileRecord;
            }
            
            if(record.get('type') == 'folder') {
                action.hide();
                return;
            }
        }
       
        for(var i=0; i < records.length; i++) {
            
            var record = records[i];
            if(record.fileRecord) {
                record = record.fileRecord;
            }
            if(!record.get('status') || (record.get('type') != 'folder' &&  record.get('status') != 'uploading' 
                    &&  record.get('status') != 'paused' && record.get('status') != 'pending')) {
                action.hide();
                return;
            }
        }
        
        action.show();
        
        for(var i=0; i < records.length; i++) {
            
            var record = records[i];
            if(record.fileRecord) {
                record = record.fileRecord;
            }
            
            if(record.get('status')) {
                action.setDisabled(false);
            }
            else {
                action.setDisabled(true);
            }
            if(record.get('status') && record.get('status') != 'paused') {
                action.setDisabled(true);
            }
            
        }   
    },
    
    /**
     * checks whether pause button shuold be enabled or disabled
     * 
     * @param action
     * @param grants
     * @param records
     */
    isPauseEnabled: function(action, grants, records) {
        
        for(var i=0; i<records.length; i++) {
            
            var record = records[i];
            if(record.fileRecord) {
                record = record.fileRecord;
            }
            
            if(record.get('type') === 'folder') {
                action.hide();
                return;
            }
        }
        
        for(var i=0; i < records.length; i++) {
            
            var record = records[i];
            if(record.fileRecord) {
                record = record.fileRecord;
            }
            
            if(!record.get('status') || (record.get('type ') != 'folder' && record.get('status') != 'paused'
                    &&  record.get('status') != 'uploading' && record.get('status') != 'pending')) {
                action.hide();
                return;
            }
        }
        
        action.show();
        
        for(var i=0; i < records.length; i++) {
            
            var record = records[i];
            if(record.fileRecord) {
                record = record.fileRecord;
            }
            
            if(record.get('status')) {
                action.setDisabled(false);
            }
            else {
                action.setDisabled(true);
            }
            if(record.get('status') && record.get('status') !=='uploading'){
                action.setDisabled(true);
            }
            
        }
    }
};

// extends Tine.widgets.tree.ContextMenu
Ext.applyIf(Tine.Filemanager.GridContextMenu, Tine.widgets.tree.ContextMenu);
/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2007-2012 Metaways Infosystems GmbH (http://www.metaways.de)
 */
Ext.ns('Tine.Filemanager.Model');
    
/**
 * @namespace   Tine.Filemanager.Model
 * @class       Tine.Filemanager.Model.Node
 * @extends     Tine.Tinebase.data.Record
 * Example record definition
 * 
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 */
Tine.Filemanager.Model.Node = Tine.Tinebase.data.Record.create(Tine.Tinebase.Model.genericFields.concat([
    { name: 'id' },
    { name: 'name' },
    { name: 'path' },
    { name: 'size' },
    { name: 'revision' },
    { name: 'type' },
    { name: 'contenttype' },
    { name: 'description' },
    { name: 'account_grants' },
    { name: 'description' },
    { name: 'object_id'},
    
    { name: 'relations' },
    { name: 'customfields' },
    { name: 'notes' },
    { name: 'tags' }
]), {
    appName: 'Filemanager',
    modelName: 'Node',
    idProperty: 'id',
    titleProperty: 'name',
    // ngettext('File', 'Files', n); gettext('File');
    recordName: 'File',
    recordsName: 'Files',
    containerProperty: null,
    // ngettext('Folder', 'Folders', n); gettext('Folder');
    containerName: 'Folder',
    containersName: 'Folders',
    
    /**
     * checks whether creating folders is allowed
     */
    isCreateFolderAllowed: function() {
        var grants = this.get('account_grants');
        
        if(!grants && this.data.id !== 'personal' && this.data.id !== 'shared') {
            return false;
        }
        else if(!grants && (this.data.id === 'personal' 
            || (this.data.id === 'shared' && (Tine.Tinebase.common.hasRight('admin', this.appName) 
            || Tine.Tinebase.common.hasRight('manage_shared_folders', this.appName))))) {
            return true;
        } else if (!grants) {
            return false;
        }
        
        return this.get('type') == 'file' ? grants.editGrant : grants.addGrant;
    },
    
    isDropFilesAllowed: function() {
        var grants = this.get('account_grants');
        if(!grants) {
            return false;
        }
        else if(!grants.addGrant) {
            return false;
        }
        return true;
    },
    
    isDragable: function() {
        var grants = this.get('account_grants');
        
        if(!grants) {
            return false;
        }
        
        if(this.get('id') === 'personal' || this.get('id') === 'shared' || this.get('id') === 'otherUsers') {
            return false;
        }
        
        return true;
    }
});
    
/**
 * create Node from File
 * 
 * @param {File} file
 */
Tine.Filemanager.Model.Node.createFromFile = function(file) {
    return new Tine.Filemanager.Model.Node({
        name: file.name ? file.name : file.fileName,  // safari and chrome use the non std. fileX props
        size: file.size || 0,
        type: 'file',
        contenttype: file.type ? file.type : file.fileType, // missing if safari and chrome 
        revision: 0
    });
};

/**
 * default ExampleRecord backend
 */
Tine.Filemanager.fileRecordBackend =  new Tine.Tinebase.data.RecordProxy({
    appName: 'Filemanager',
    modelName: 'Node',
    recordClass: Tine.Filemanager.Model.Node,
    
    /**
     * creating folder
     * 
     * @param name      folder name
     * @param options   additional options
     * @returns
     */
    createFolder: function(name, options) {
        
        options = options || {};
        var params = {
                application : this.appName,
                filename : name,
                type : 'folder',
                method : this.appName + ".createNode"  
        };
        
        options.params = params;
        
        options.beforeSuccess = function(response) {
            var folder = this.recordReader(response);
            folder.set('client_access_time', new Date());
            return [folder];
        };
        
        options.success = function(result){
            var app = Tine.Tinebase.appMgr.get(Tine.Filemanager.fileRecordBackend.appName);
            var grid = app.getMainScreen().getCenterPanel();
            var nodeData = Ext.util.JSON.decode(result);
            var newNode = app.getMainScreen().getWestPanel().getContainerTreePanel().createTreeNode(nodeData, parentNode);
            
            var parentNode = grid.currentFolderNode;
            if(parentNode) {
                parentNode.appendChild(newNode);
            }
            grid.getStore().reload();
//            this.fireEvent('containeradd', nodeData);
        };
        
        return this.doXHTTPRequest(options);
    },
    
    /**
     * is automatically called in generic GridPanel
     */
    saveRecord : function(record, request) {
        if(record.hasOwnProperty('fileRecord')) {
            return;
        } else {
            Tine.Tinebase.data.RecordProxy.prototype.saveRecord.call(this, record, request);
        }
    },

    /**
     * deleting file or folder
     * 
     * @param items     files/folders to delete
     * @param options   additional options
     * @returns
     */
    deleteItems: function(items, options) {
        options = options || {};
        
        var filenames = new Array();
        var nodeCount = items.length;
        for(var i=0; i<nodeCount; i++) {
            filenames.push(items[i].data.path );
        }
        
        var params = {
            application: this.appName,
            filenames: filenames,
            method: this.appName + ".deleteNodes",
            timeout: 300000 // 5 minutes
        };
        
        options.params = params;
        
        options.beforeSuccess = function(response) {
            var folder = this.recordReader(response);
            folder.set('client_access_time', new Date());
            return [folder];
        };
        
        options.success = (function(result){
            var app = Tine.Tinebase.appMgr.get(Tine.Filemanager.fileRecordBackend.appName),
                grid = app.getMainScreen().getCenterPanel(),
                treePanel = app.getMainScreen().getWestPanel().getContainerTreePanel(),
                nodeData = this.items;
            
            for(var i=0; i<nodeData.length; i++) {
                var treeNode = treePanel.getNodeById(nodeData[i].id);
                if(treeNode) {
                    treeNode.parentNode.removeChild(treeNode);
                }
            }
            
            grid.getStore().remove(nodeData);
            grid.selectionModel.deselectRange(0, grid.getStore().getCount());
            grid.pagingToolbar.refresh.enable();
//            this.fireEvent('containerdelete', nodeData);
            
        }).createDelegate({items: items});
        
        return this.doXHTTPRequest(options);
    },
    
    /**
     * copy/move folder/files to a folder
     * 
     * @param items files/folders to copy
     * @param targetPath
     * @param move
     */
    
    copyNodes : function(items, target, move, params) {
        
        var containsFolder = false,
            message = '',
            app = Tine.Tinebase.appMgr.get(Tine.Filemanager.fileRecordBackend.appName);
        
        
        if(!params) {
        
            if(!target || !items || items.length < 1) {
                return false;
            }
            
            var sourceFilenames = new Array(),
            destinationFilenames = new Array(),
            forceOverwrite = false,
            treeIsTarget = false,
            treeIsSource = false,
            targetPath;
            
            if(target.data) {
                targetPath = target.data.path;
            }
            else {
                targetPath = target.attributes.path;
                treeIsTarget = true;
            }
            
            for(var i=0; i<items.length; i++) {
                
                var item = items[i];
                var itemData = item.data;
                if(!itemData) {
                    itemData = item.attributes;
                    treeIsSource = true;
                }
                sourceFilenames.push(itemData.path);
                
                var itemName = itemData.name;
                if(typeof itemName == 'object') {
                    itemName = itemName.name;
                }
                
                destinationFilenames.push(targetPath + '/' + itemName);
                if(itemData.type == 'folder') {
                    containsFolder = true;
                }
            };
            
            var method = "Filemanager.copyNodes",
                message = app.i18n._('Copying data .. {0}');
            if(move) {
                method = "Filemanager.moveNodes";
                message = app.i18n._('Moving data .. {0}');
            }
            
            params = {
                    application: this.appName,
                    sourceFilenames: sourceFilenames,
                    destinationFilenames: destinationFilenames,
                    forceOverwrite: forceOverwrite,
                    method: method
            };
            
        }
        else {
            message = app.i18n._('Copying data .. {0}');
            if(params.method == 'Filemanager.moveNodes') {
                message = app.i18n._('Moving data .. {0}');
            }
        }
        
        this.loadMask = new Ext.LoadMask(app.getMainScreen().getCenterPanel().getEl(), {msg: String.format(_('Please wait')) + '. ' + String.format(message, '' )});
        app.getMainScreen().getWestPanel().setDisabled(true);
        app.getMainScreen().getNorthPanel().setDisabled(true);
        this.loadMask.show();
        
        Ext.Ajax.request({
            params: params,
            timeout: 300000, // 5 minutes
            scope: this,
            success: function(result, request){
                
                this.loadMask.hide();
                app.getMainScreen().getWestPanel().setDisabled(false);
                app.getMainScreen().getNorthPanel().setDisabled(false);
                
                var nodeData = Ext.util.JSON.decode(result.responseText),
                    treePanel = app.getMainScreen().getWestPanel().getContainerTreePanel(),
                    grid = app.getMainScreen().getCenterPanel();
             
                // Tree refresh
                if(treeIsTarget) {
                    
                    for(var i=0; i<items.length; i++) {
                        
                        var nodeToCopy = items[i];
                        
                        if(nodeToCopy.data && nodeToCopy.data.type !== 'folder') {
                            continue;
                        }
                        
                        if(move) {
                            var copiedNode = treePanel.cloneTreeNode(nodeToCopy, target),
                                nodeToCopyId = nodeToCopy.id,
                                removeNode = treePanel.getNodeById(nodeToCopyId);
                            
                            if(removeNode && removeNode.parentNode) {
                                removeNode.parentNode.removeChild(removeNode);
                            }
                            
                            target.appendChild(copiedNode);
                            copiedNode.setId(nodeData[i].id);
                        } 
                        else {
                            var copiedNode = treePanel.cloneTreeNode(nodeToCopy, target);
                            target.appendChild(copiedNode);
                            copiedNode.setId(nodeData[i].id);
                            
                        }
                    }
                }
               
                // Grid refresh
                grid.getStore().reload();
            },
            failure: function(response, request) {
                var nodeData = Ext.util.JSON.decode(response.responseText),
                    request = Ext.util.JSON.decode(request.jsonData);
                
                this.loadMask.hide();
                app.getMainScreen().getWestPanel().setDisabled(false);
                app.getMainScreen().getNorthPanel().setDisabled(false);
                
                Tine.Filemanager.fileRecordBackend.handleRequestException(nodeData.data, request);
            }
        });
               
    },
    
    /**
     * upload file 
     * 
     * @param {} params Request parameters
     * @param String uploadKey
     * @param Boolean addToGridStore 
     */
    createNode: function(params, uploadKey, addToGridStore) {
        var app = Tine.Tinebase.appMgr.get('Filemanager'),
            grid = app.getMainScreen().getCenterPanel(),
            gridStore = grid.getStore();
        
        params.application = 'Filemanager';
        params.method = 'Filemanager.createNode';
        params.uploadKey = uploadKey;
        params.addToGridStore = addToGridStore;
        
        var onSuccess = (function(result, request){
            
            var nodeData = Ext.util.JSON.decode(response.responseText),
                fileRecord = Tine.Tinebase.uploadManager.upload(this.uploadKey);
            
            if(addToGridStore) {
                var recordToRemove = gridStore.query('name', fileRecord.get('name'));
                if(recordToRemove.items[0]) {
                    gridStore.remove(recordToRemove.items[0]);
                }
                
                fileRecord = Tine.Filemanager.fileRecordBackend.updateNodeRecord(nodeData[i], fileRecord);
                var nodeRecord = new Tine.Filemanager.Model.Node(nodeData[i]);
                
                nodeRecord.fileRecord = fileRecord;
                gridStore.add(nodeRecord);
                
            }
        }).createDelegate({uploadKey: uploadKey, addToGridStore: addToGridStore});
        
        var onFailure = (function(response, request) {
            
            var nodeData = Ext.util.JSON.decode(response.responseText),
                request = Ext.util.JSON.decode(request.jsonData);
            
            nodeData.data.uploadKey = this.uploadKey;
            nodeData.data.addToGridStore = this.addToGridStore;
            Tine.Filemanager.fileRecordBackend.handleRequestException(nodeData.data, request);
            
        }).createDelegate({uploadKey: uploadKey, addToGridStore: addToGridStore});
        
        Ext.Ajax.request({
            params: params,
            timeout: 300000, // 5 minutes
            scope: this,
            success: onSuccess || Ext.emptyFn,
            failure: onFailure || Ext.emptyFn
        });
    },
    
    /**
     * upload files
     * 
     * @param {} params Request parameters
     * @param [] uploadKeyArray
     * @param Boolean addToGridStore 
     */
    createNodes: function(params, uploadKeyArray, addToGridStore) {
        var app = Tine.Tinebase.appMgr.get('Filemanager'),
            grid = app.getMainScreen().getCenterPanel(),
            gridStore = grid.store;
        
        params.application = 'Filemanager';
        params.method = 'Filemanager.createNodes';
        params.uploadKeyArray = uploadKeyArray;
        params.addToGridStore = addToGridStore;
        
        
        var onSuccess = (function(response, request){
            
            var nodeData = Ext.util.JSON.decode(response.responseText);
            
            for(var i=0; i<this.uploadKeyArray.length; i++) {
                var fileRecord = Tine.Tinebase.uploadManager.upload(this.uploadKeyArray[i]);
                
                if(addToGridStore) {
                    fileRecord = Tine.Filemanager.fileRecordBackend.updateNodeRecord(nodeData[i], fileRecord);
                    var nodeRecord = new Tine.Filemanager.Model.Node(nodeData[i]);
                    
                    nodeRecord.fileRecord = fileRecord;
                    
                    var existingRecordIdx = gridStore.find('name', fileRecord.get('name'));
                    if(existingRecordIdx > -1) {
                        gridStore.removeAt(existingRecordIdx);
                        gridStore.insert(existingRecordIdx, nodeRecord);
                    } else {
                        gridStore.add(nodeRecord);
                    }
                }
            }
            
        }).createDelegate({uploadKeyArray: uploadKeyArray, addToGridStore: addToGridStore});
        
        var onFailure = (function(response, request) {
            
            var nodeData = Ext.util.JSON.decode(response.responseText),
                request = Ext.util.JSON.decode(request.jsonData);
            
            nodeData.data.uploadKeyArray = this.uploadKeyArray;
            nodeData.data.addToGridStore = this.addToGridStore;
            Tine.Filemanager.fileRecordBackend.handleRequestException(nodeData.data, request);
            
        }).createDelegate({uploadKeyArray: uploadKeyArray, addToGridStore: addToGridStore});
        
        Ext.Ajax.request({
            params: params,
            timeout: 300000, // 5 minutes
            scope: this,
            success: onSuccess || Ext.emptyFn,
            failure: onFailure || Ext.emptyFn
        });
        
        
    },
    
    /**
     * exception handler for this proxy
     * 
     * @param {Tine.Exception} exception
     */
    handleRequestException: function(exception, request) {
        Tine.Filemanager.handleRequestException(exception, request);
    },
    
    /**
     * updates given record with nodeData from from response
     */
    updateNodeRecord : function(nodeData, nodeRecord) {
        
        for(var field in nodeData) {
            nodeRecord.set(field, nodeData[field]);
        };
        
        return nodeRecord;
    }
    

});


/**
 * get filtermodel of contact model
 * 
 * @namespace Tine.Filemanager.Model
 * @static
 * @return {Object} filterModel definition
 */ 
Tine.Filemanager.Model.Node.getFilterModel = function() {
    var app = Tine.Tinebase.appMgr.get('Filemanager');
       
    return [
        {label : _('Quick search'), field : 'query', operators : [ 'contains' ]}, 
//        {label: app.i18n._('Type'), field: 'type'}, // -> should be a combo
        {label: app.i18n._('Contenttype'), field: 'contenttype'},
        {label: app.i18n._('Creation Time'), field: 'creation_time', valueType: 'date'},
        {filtertype : 'tine.filemanager.pathfiltermodel', app : app}, 
        {filtertype : 'tinebase.tag', app : app} 
    ];
};/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2010-2012 Metaways Infosystems GmbH (http://www.metaways.de)
 * 
 */
Ext.ns('Tine.Filemanager');

/**
 * @namespace   Tine.widgets.container
 * @class       Tine.Filemanager.PathFilterModel
 * @extends     Tine.widgets.grid.FilterModel
 * 
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * 
 * @TODO make valueRenderer a path picker widget
 */
Tine.Filemanager.PathFilterModel = Ext.extend(Tine.widgets.grid.FilterModel, {
    /**
     * @cfg {Tine.Tinebase.Application} app
     */
    app: null,
    
    /**
     * @cfg {Array} operators allowed operators
     */
    operators: ['equals'],
    
    /**
     * @cfg {String} field path
     */
    field: 'path',
    
    /**
     * @cfg {String} defaultOperator default operator, one of <tt>{@link #operators} (defaults to equals)
     */
    defaultOperator: 'equals',
    
    /**
     * @cfg {String} defaultValue default value (defaults to all)
     */
    defaultValue: '/',
    
    /**
     * @private
     */
    initComponent: function() {
        this.label = this.app.i18n._('path');
        
        Tine.Filemanager.PathFilterModel.superclass.initComponent.call(this);
    },
    
    /**
     * value renderer
     * 
     * @param {Ext.data.Record} filter line
     * @param {Ext.Element} element to render to 
     */
    valueRenderer: function(filter, el) {
        var value = new Ext.ux.form.ClearableTextField({
            filter: filter,
            width: this.filterValueWidth,
            id: 'tw-ftb-frow-valuefield-' + filter.id,
            renderTo: el,
            value: filter.data.value ? filter.data.value : this.defaultValue,
            emptyText: this.emptyText,
            value: filter.data.value
        });
        
        value.on('specialkey', function(field, e){
            if(e.getKey() == e.ENTER){
                this.onFiltertrigger();
            }
        }, this);
                
        value.origSetValue = value.setValue.createDelegate(value);
        value.setValue = function(value) {
            if (value && value.path) {
                value = value.path;
            }
            else if(Ext.isString(value) && (!value.charAt(0) || value.charAt(0) != '/')) {
                value = '/' + value;
            }
            
            return this.origSetValue(value);
        };
        
        return value;
    }
});

Tine.widgets.grid.FilterToolbar.FILTERS['tine.filemanager.pathfiltermodel'] = Tine.Filemanager.PathFilterModel;

/*
 * Tine 2.0
 * Filemanager combo box and store
 * 
 * @package     Filemanager
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Alexander Stintzing <a.stintzing@metaways.de>
 * @copyright   Copyright (c) 2012 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */

Ext.ns('Tine.Filemanager');

/**
 * Node selection combo box
 * 
 * @namespace   Tine.Filemanager
 * @class       Tine.Filemanager.SearchCombo
 * @extends     Ext.form.ComboBox
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Alexander Stintzing <a.stintzing@metaways.de>
 * @copyright   Copyright (c) 2012 Metaways Infosystems GmbH (http://www.metaways.de)
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Filemanager.SearchCombo
 */
Tine.Filemanager.SearchCombo = Ext.extend(Tine.Tinebase.widgets.form.RecordPickerComboBox, {
    
    allowBlank: false,
    itemSelector: 'div.search-item',
    minListWidth: 200,
    
    //private
    initComponent: function(){
        this.recordClass = Tine.Filemanager.Model.Node;
        this.recordProxy = Tine.Filemanager.recordBackend;
        
        this.initTemplate();
        Tine.Filemanager.SearchCombo.superclass.initComponent.call(this);
    },
    
    /**
     * use beforequery to set query filter
     * 
     * @param {Event} qevent
     */
    onBeforeQuery: function(qevent) {
        Tine.Filemanager.SearchCombo.superclass.onBeforeQuery.apply(this, arguments);
        this.store.baseParams.filter.push(
            {field: 'recursive', operator: 'equals', value: true }
        );
        this.store.baseParams.filter.push(
            {field: 'path', operator: 'equals', value: '/' }
        );
    },
    
    /**
     * init template
     * @private
     */
    initTemplate: function() {
        // Custom rendering Template
        // TODO move style def to css ?
        if (! this.tpl) {
            this.tpl = new Ext.XTemplate(
                '<tpl for="."><div class="search-item">',
                    '<table cellspacing="0" cellpadding="2" border="0" style="font-size: 11px;" width="100%">',
                        '<tr>',
                            '<td ext:qtip="{[this.renderPathName(values)]}" style="height:16px">{[this.renderFileName(values)]}</td>',
                        '</tr>',
                    '</table>',
                '</div></tpl>',
                {
                    renderFileName: function(values) {
                        return Ext.util.Format.htmlEncode(values.name);
                    },
                    renderPathName: function(values) {
                        return Ext.util.Format.htmlEncode(values.path.replace(values.name, ''));
                    }
                    
                }
            );
        }
    },
    
    getValue: function() {
            return Tine.Filemanager.SearchCombo.superclass.getValue.call(this);
    },

    setValue: function (value) {
        return Tine.Filemanager.SearchCombo.superclass.setValue.call(this, value);
    }

});

Tine.widgets.form.RecordPickerManager.register('Filemanager', 'Node', Tine.Filemanager.SearchCombo);
/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2010-2012 Metaways Infosystems GmbH (http://www.metaways.de)
 * 
 */
Ext.ns('Tine.Filemanager');

/**
 * filter plugin for container tree
 * 
 * @namespace Tine.widgets.tree
 * @class     Tine.Filemanager.PathFilterPlugin
 * @extends   Tine.widgets.grid.FilterPlugin
 */
Tine.Filemanager.PathFilterPlugin = Ext.extend(Tine.widgets.tree.FilterPlugin, {
    
    /**
     * select tree node(s)
     * 
     * @param {String} value
     */
    selectValue: function(value) {
        var values = Ext.isArray(value) ? value : [value];
        Ext.each(values, function(value) {
            var path = Ext.isString(value) ? value : (value ? value.path : '') || '/',
                treePath = this.treePanel.getTreePath(path);
            
            this.selectPath.call(this.treePanel, treePath, null, function() {
                // mark this expansion as done and check if all are done
                value.isExpanded = true;
                var allValuesExpanded = true;
                Ext.each(values, function(v) {
                    allValuesExpanded &= v.isExpanded;
                }, this);
                
                if (allValuesExpanded) {
                    this.treePanel.getSelectionModel().resumeEvents();
                    
                    // @TODO remove this code when fm is cleaned up conceptually
                    //       currentFolderNode -> currentFolder
                    this.treePanel.updateActions(this.treePanel.getSelectionModel(), this.treePanel.getSelectionModel().getSelectedNode());
                    Tine.Tinebase.appMgr.get('Filemanager').getMainScreen().getCenterPanel().currentFolderNode = this.treePanel.getSelectionModel().getSelectedNode();
                }
            }.createDelegate(this), true);
        }, this);
    }
});
/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Alexander Stintzing <a.stintzing@metaways.de>
 * @copyright   Copyright (c) 2012 Metaways Infosystems GmbH (http://www.metaways.de)
 */
Ext.ns('Tine.Filemanager');

/**
 * @namespace   Tine.Filemanager
 * @class       Tine.Filemanager.NodeEditDialog
 * @extends     Tine.widgets.dialog.EditDialog
 * 
 * <p>Node Compose Dialog</p>
 * <p></p>
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Alexander Stintzing <a.stintzing@metaways.de>
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Filemanager.NodeEditDialog
 */
Tine.Filemanager.NodeEditDialog = Ext.extend(Tine.widgets.dialog.EditDialog, {
    
    /**
     * @private
     */
    windowNamePrefix: 'NodeEditWindow_',
    appName: 'Filemanager',
    recordClass: Tine.Filemanager.Model.Node,
    recordProxy: Tine.Filemanager.fileRecordBackend,
    tbarItems: [{xtype: 'widget-activitiesaddbutton'}],
    evalGrants: true,
    showContainerSelector: false,
    
    initComponent: function() {
        this.app = Tine.Tinebase.appMgr.get('Filemanager');
        this.downloadAction = new Ext.Action({
            requiredGrant: 'readGrant',
            allowMultiple: false,
            actionType: 'download',
            text: this.app.i18n._('Save locally'),
            handler: this.onDownload,
            iconCls: 'action_filemanager_save_all',
            disabled: false,
            scope: this
        });
        
        this.tbarItems.push(this.downloadAction);
        Tine.Filemanager.NodeEditDialog.superclass.initComponent.call(this);
    },
    
    /**
     * download file
     */
    onDownload: function() {
        var downloader = new Ext.ux.file.Download({
            params: {
                method: 'Filemanager.downloadFile',
                requestType: 'HTTP',
                path: '',
                id: this.record.get('id')
            }
        }).start();
    },
    
    /**
     * returns dialog
     * @return {Object}
     * @private
     */
    getFormItems: function() {
        var formFieldDefaults = {
            xtype:'textfield',
            anchor: '100%',
            labelSeparator: '',
            columnWidth: .5,
            readOnly: true,
            disabled: true
        };
        
        return {
            xtype: 'tabpanel',
            border: false,
            plain:true,
            plugins: [{
                ptype : 'ux.tabpanelkeyplugin'
            }],
            activeTab: 0,
            border: false,
            items:[{
                title: this.app.i18n._('Node'),
                autoScroll: true,
                border: false,
                frame: true,
                layout: 'border',
                items: [{
                    region: 'center',
                    layout: 'hfit',
                    border: false,
                    items: [{
                        xtype: 'fieldset',
                        layout: 'hfit',
                        autoHeight: true,
                        title: this.app.i18n._('Node'),
                        items: [{
                            xtype: 'columnform',
                            labelAlign: 'top',
                            formDefaults: formFieldDefaults,
                            items: [[{
                                    fieldLabel: this.app.i18n._('Name'),
                                    name: 'name',
                                    allowBlank: false,
                                    readOnly: false,
                                    columnWidth: .75,
                                    disabled: false
                                }, {
                                    fieldLabel: this.app.i18n._('Type'),
                                    name: 'contenttype',
                                    columnWidth: .25
                                }],[
                                Tine.widgets.form.RecordPickerManager.get('Addressbook', 'Contact', {
                                    userOnly: true,
                                    useAccountRecord: true,
                                    blurOnSelect: true,
                                    fieldLabel: this.app.i18n._('Created By'),
                                    name: 'created_by'
                                }), {
                                    fieldLabel: this.app.i18n._('Creation Time'),
                                    name: 'creation_time',
                                    xtype: 'datefield'
                                }
                                ],[
                                Tine.widgets.form.RecordPickerManager.get('Addressbook', 'Contact', {
                                    userOnly: true,
                                    useAccountRecord: true,
                                    blurOnSelect: true,
                                    fieldLabel: this.app.i18n._('Modified By'),
                                    name: 'last_modified_by'
                                }), {
                                    fieldLabel: this.app.i18n._('Last Modified'),
                                    name: 'last_modified_time',
                                    xtype: 'datefield'
                                }
                                ]]
                        }]
                    }
                    
                    ]
                }, {
                    // activities and tags
                    layout: 'accordion',
                    animate: true,
                    region: 'east',
                    width: 210,
                    split: true,
                    collapsible: true,
                    collapseMode: 'mini',
                    header: false,
                    margins: '0 5 0 5',
                    border: true,
                    items: [
                        new Ext.Panel({
                            title: this.app.i18n._('Description'),
                            iconCls: 'descriptionIcon',
                            layout: 'form',
                            labelAlign: 'top',
                            border: false,
                            items: [{
                                style: 'margin-top: -4px; border 0px;',
                                labelSeparator: '',
                                xtype: 'textarea',
                                name: 'description',
                                hideLabel: true,
                                grow: false,
                                preventScrollbars: false,
                                anchor: '100% 100%',
                                emptyText: this.app.i18n._('Enter description'),
                                requiredGrant: 'editGrant'
                            }]
                        }),
                        new Tine.widgets.activities.ActivitiesPanel({
                            app: 'Filemanager',
                            showAddNoteForm: false,
                            border: false,
                            bodyStyle: 'border:1px solid #B5B8C8;'
                        }),
                        new Tine.widgets.tags.TagPanel({
                            app: 'Filemanager',
                            border: false,
                            bodyStyle: 'border:1px solid #B5B8C8;'
                        })
                    ]
                }]
            }, 
            new Tine.widgets.activities.ActivitiesTabPanel({
                app: this.appName,
                record_id: this.record.id,
                record_model: this.appName + '_Model_' + this.recordClass.getMeta('modelName')
                })
            ]
        };
    }
});

/**
 * Filemanager Edit Popup
 * 
 * @param   {Object} config
 * @return  {Ext.ux.Window}
 */
Tine.Filemanager.NodeEditDialog.openWindow = function (config) {
    var id = (config.record && config.record.id) ? config.record.id : 0;
    var window = Tine.WindowFactory.getWindow({
        width: 800,
        height: 570,
        name: Tine.Filemanager.NodeEditDialog.prototype.windowNamePrefix + id,
        contentPanelConstructor: 'Tine.Filemanager.NodeEditDialog',
        contentPanelConstructorConfig: config
    });
    return window;
};
/*
 * Tine 2.0
 * 
 * @package     Tinebase
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2010-2012 Metaways Infosystems GmbH (http://www.metaways.de)
 */

Ext.ns('Tine.Filemanager');

/**
 * @namespace Tine.Filemanager
 * @class Tine.Filemanager.NodeTreePanel
 * @extends Tine.widgets.container.TreePanel
 * 
 * @author Martin Jatho <m.jatho@metaways.de>
 */

Tine.Filemanager.NodeTreePanel = function(config) {
    Ext.apply(this, config);
    
    this.addEvents(
        /**
         * @event containeradd
         * Fires when a folder was added
         * @param {folder} the new folder
         */
        'containeradd',
        /**
         * @event containerdelete
         * Fires when a folder got deleted
         * @param {folder} the deleted folder
         */
        'containerdelete',
        /**
         * @event containerrename
         * Fires when a folder got renamed
         * @param {folder} the renamed folder
         */
        'containerrename'
    );
    
    Tine.Filemanager.NodeTreePanel.superclass.constructor.call(this);
};

Ext.extend(Tine.Filemanager.NodeTreePanel, Tine.widgets.container.TreePanel, {
    
    filterMode : 'filterToolbar',
    
    recordClass : Tine.Filemanager.Model.Node,
    
    allowMultiSelection : false, 
    
    defaultContainerPath: '/personal',
    
    ddGroup: 'fileDDGroup',
    
    enableDD: true,
       
    initComponent: function() {
        
        this.on('containeradd', this.onFolderAdd, this);
        this.on('containerrename', this.onFolderRename, this);
        this.on('containerdelete', this.onFolderDelete, this);
        this.on('nodedragover', this.onNodeDragOver, this);
        
        Tine.Tinebase.uploadManager.on('update', this.onUpdate);
        
        Tine.Filemanager.NodeTreePanel.superclass.initComponent.call(this);
        
        // init drop zone
        this.dropConfig = {
            ddGroup: this.ddGroup || 'fileDDGroup',
            appendOnly: this.ddAppendOnly === true,
            /**
             * @todo check acl!
             */
            onNodeOver : function(n, dd, e, data) {
                
                var preventDrop = false,
                    selectionContainsFiles = false;
                
                if(dd.dragData.selections) {
                    for(var i=0; i<dd.dragData.selections.length; i++) {
                        if(n.node.id == dd.dragData.selections[i].id) {
                            preventDrop = true;
                        }
                        if(dd.dragData.selections[i].data.type == 'file') {
                            selectionContainsFiles = true;
                        }
                    }
                }
                else if(dd.dragData.node && dd.dragData.node.id == n.node.id) {
                    preventDrop = true;
                } 
                
                if(selectionContainsFiles && !n.node.attributes.account_grants) {
                    preventDrop = true;
                }
                
                if(n.node.isAncestor(dd.dragData.node)) {
                    preventDrop = true;
                }
                
                return n.node.attributes.nodeRecord.isCreateFolderAllowed() 
                    && (!dd.dragData.node || dd.dragData.node.attributes.nodeRecord.isDragable())
                    && !preventDrop ? 'x-dd-drop-ok' : false;
            },
            
            isValidDropPoint: function(n, op, dd, e){
                
                var preventDrop = false,
                selectionContainsFiles = false;
                
                if(dd.dragData.selections) {
                    for(var i=0; i<dd.dragData.selections.length; i++) {
                        if(n.node.id == dd.dragData.selections[i].id) {
                            preventDrop = true;
                        }
                        if(dd.dragData.selections[i].data.type == 'file') {
                            selectionContainsFiles = true;
                        }
                    }
                }
                else if(dd.dragData.node && dd.dragData.node.id == n.node.id) {
                    preventDrop = true;
                } 
                
                if(selectionContainsFiles && !n.node.attributes.account_grants) {
                    preventDrop = true;
                }
                
                if(n.node.isAncestor(dd.dragData.node)) {
                    preventDrop = true;
                }
                
                return n.node.attributes.nodeRecord.isCreateFolderAllowed()
                        && (!dd.dragData.node || dd.dragData.node.attributes.nodeRecord.isDragable())
                        && !preventDrop;
            },
            completeDrop: function(de) {
                var ns = de.dropNode, p = de.point, t = de.target;
                t.ui.endDrop();
                this.tree.fireEvent("nodedrop", de);
            }
        };
        
        this.dragConfig = {
            ddGroup: this.ddGroup || 'fileDDGroup',
            scroll: this.ddScroll,
            /**
             * tree node dragzone modified, dragged node doesn't get selected
             * 
             * @param e
             */
            onInitDrag: function(e) {
                var data = this.dragData;
                this.tree.eventModel.disable();
                this.proxy.update("");
                data.node.ui.appendDDGhost(this.proxy.ghost.dom);
                this.tree.fireEvent("startdrag", this.tree, data.node, e);
            }
        };
        
        this.plugins = this.plugins || [];
        this.plugins.push({
            ptype : 'ux.browseplugin',
            enableFileDialog: false,
            multiple : true,
            handler : this.dropIntoTree
        });
    },
    
    /**
     * Tine.widgets.tree.FilterPlugin
     * returns a filter plugin to be used in a grid
     */
    // Tine.widgets.tree.FilterPlugin
    // Tine.Filemanager.PathFilterPlugin
    getFilterPlugin: function() {
        if (!this.filterPlugin) {
            this.filterPlugin = new Tine.Filemanager.PathFilterPlugin({
                treePanel: this,
                field: 'path',
                nodeAttributeField: 'path'
            });
        }
        
        return this.filterPlugin;
    },
    
    /**
     * returns the personal root path
     * @returns {String}
     */
    getRootPath: function() {
        return Tine.Tinebase.container.getMyFileNodePath();
    },
    
    /**
     * returns params for async request
     * 
     * @param {Ext.tree.TreeNode} node
     * @return {Object}
     */
    onBeforeLoad: function(node) {
        
        var path = node.attributes.path;
        var type = Tine.Tinebase.container.path2type(path);
        var owner = Tine.Tinebase.container.pathIsPersonalNode(path);
        var loginName = Tine.Tinebase.registry.get('currentAccount').accountLoginName;
        
        if (type === 'personal' && owner != loginName) {
            type = 'otherUsers';
        }
        
        var newPath = path;
        
        if (type === 'personal' && owner) {
            var pathParts = path.toString().split('/');
            newPath = '/' + pathParts[1] + '/' + loginName;
            if(pathParts[3]) {
                newPath += '/' + pathParts[3];
            }
        }
        
        var params = {
            method: 'Filemanager.searchNodes',
            application: this.app.appName,
            owner: owner,
            filter: [
                     {field: 'path', operator:'equals', value: newPath},
                     {field: 'type', operator:'equals', value: 'folder'}
                     ],
            paging: {dir: 'ASC', limit: 50, sort: 'name', start: 0}
        };
        
        return params;
    },
    
    onBeforeCreateNode: function(attr) {
        Tine.Filemanager.NodeTreePanel.superclass.onBeforeCreateNode.apply(this, arguments);
        
        attr.leaf = false;
        
        // use name as ids to make pathfilter work
        if (attr.path && attr.created_by) {
            var keys = attr.path.split('/');
            attr.id = keys.pop();
        }
        
        if(attr.name && typeof attr.name == 'object') {
            Ext.apply(attr, {
                text: Ext.util.Format.htmlEncode(attr.name.name),
                qtip: Tine.Tinebase.common.doubleEncode(attr.name.name)
            });
        }
        
        // copy 'real' data to a node record NOTE: not a full record as we have no record reader here
        var nodeData = Ext.copyTo({}, attr, Tine.Filemanager.Model.Node.getFieldNames());
        attr.nodeRecord = new Tine.Filemanager.Model.Node(nodeData);
    },
    
    /**
     * initiates tree context menues
     * 
     * @private
     */
    initContextMenu: function() {
        
        this.contextMenuUserFolder = Tine.widgets.tree.ContextMenu.getMenu({
            nodeName: this.app.i18n._(this.containerName),
            actions: ['add', 'reload', 'delete', 'rename', 'properties'],
            scope: this,
            backend: 'Filemanager',
            backendModel: 'Node'
        });
            
        this.contextMenuRootFolder = Tine.widgets.tree.ContextMenu.getMenu({
            nodeName: this.app.i18n._(this.containerName),
            actions: ['add', 'reload'],
            scope: this,
            backend: 'Filemanager',
            backendModel: 'Node'
        });
        
        this.contextMenuOtherUserFolder = Tine.widgets.tree.ContextMenu.getMenu({
            nodeName: this.app.i18n._(this.containerName),
            actions: ['reload'],
            scope: this,
            backend: 'Filemanager',
            backendModel: 'Node'
        });
        
        this.contextMenuContainerFolder = Tine.widgets.tree.ContextMenu.getMenu({
            nodeName: this.app.i18n._(this.containerName),
            actions: ['add', 'reload', 'delete', 'rename', 'grants', 'properties'],
            scope: this,
            backend: 'Filemanager',
            backendModel: 'Node'
        });
        
        this.contextMenuReloadFolder = Tine.widgets.tree.ContextMenu.getMenu({
            nodeName: this.app.i18n._(this.containerName),
            actions: ['reload', 'properties'],
            scope: this,
            backend: 'Filemanager',
            backendModel: 'Node'
        });
    },
    
    /**
     * @private
     * - select default path
     */
    afterRender: function() {
        Tine.Filemanager.NodeTreePanel.superclass.afterRender.call(this);
    },
    
    /**
     * show context menu
     * 
     * @param {Ext.tree.TreeNode} node
     * @param {Ext.EventObject} event
     */
    onContextMenu: function(node, event) {
        
        var currentAccount = Tine.Tinebase.registry.get('currentAccount');
        
        this.ctxNode = node;
        var container = node.attributes.nodeRecord.data,
            path = container.path;
        
        if (! Ext.isString(path) || node.isRoot) {
            return;
        }
        
        Tine.log.debug('Tine.Filemanager.NodeTreePanel::onContextMenu - context node:');
        Tine.log.debug(node);
        
        if (node.id == 'otherUsers' || (node.parentNode && node.parentNode.id == 'otherUsers')) {
            this.contextMenuOtherUserFolder.showAt(event.getXY());
        } else if (node.id == 'personal' || node.id == 'shared') {
            this.contextMenuRootFolder.showAt(event.getXY());
        } else if (path.match(/^\/shared/) && (Tine.Tinebase.common.hasRight('admin', this.app.appName) 
                || Tine.Tinebase.common.hasRight('manage_shared_folders', this.app.appName))) {
            if (typeof container.name == 'object') {
                this.contextMenuContainerFolder.showAt(event.getXY());
            } else {
                this.contextMenuUserFolder.showAt(event.getXY());
            }
        } else if (path.match(/^\/shared/)){
            this.contextMenuReloadFolder.showAt(event.getXY());
        } else if (path.match(/^\/personal/) && path.match('/personal/' + currentAccount.accountLoginName)) {
            if (typeof container.name == 'object') {
                this.contextMenuContainerFolder.showAt(event.getXY());
            } else {
                this.contextMenuUserFolder.showAt(event.getXY());
            }
        } else if (path.match(/^\/personal/) && container.account_grants) {
            this.contextMenuUserFolder.showAt(event.getXY());
        }
    },
    
    /**
     * updates grid actions
     * @todo move to grid / actionUpdater
     * 
     * @param {} sm     SelectionModel
     * @param {Ext.tree.TreeNode} node
     */
    updateActions: function(sm, node) {
        var grid = this.app.getMainScreen().getCenterPanel();
        
        grid.action_deleteRecord.disable();
        grid.action_upload.disable();
        
        if(!!node && !!node.isRoot) {
            grid.action_goUpFolder.disable();
        }
        else {
            grid.action_goUpFolder.enable();
        }
                
        if(node && node.attributes && node.attributes.nodeRecord.isCreateFolderAllowed()) {
            grid.action_createFolder.enable();
        }
        else {
            grid.action_createFolder.disable();
        }
        
        if(node && node.attributes && node.attributes.nodeRecord.isDropFilesAllowed()) {
            grid.action_upload.enable();
        }
        else {
            grid.action_upload.disable();
        }
    },
    
    /**
     * called when tree selection changes
     * 
     * @param {} sm     SelectionModel
     * @param {Ext.tree.TreeNode} node
     */
    onSelectionChange: function(sm, node) {
        this.updateActions(sm, node);
        var grid = this.app.getMainScreen().getCenterPanel();
        
        grid.currentFolderNode = node;
        Tine.Filemanager.NodeTreePanel.superclass.onSelectionChange.call(this, sm, node);
    
    },
    
    /**
     * convert filesystem path to treePath
     * 
     * NOTE: only the first depth gets converted!
     *       fs pathes of not yet loaded tree nodes can't be converted!
     * 
     * @param {String} containerPath
     * @return {String} tree path
     */
    getTreePath: function(path) {
        var treePath = '/' + this.getRootNode().id;
        
        if (path && path != '/') {
            if (path == '/personal') {
                treePath += '/otherUsers';
            }
            
            treePath += String(path).replace('personal/'  + Tine.Tinebase.registry.get('currentAccount').accountLoginName, 'personal');
        }
        
        return treePath;
    },
    
    /**
     * Expands a specified path in this TreePanel. A path can be retrieved from a node with {@link Ext.data.Node#getPath}
     * 
     * NOTE: path does not consist of id's starting from the second depth
     * 
     * @param {String} path
     * @param {String} attr (optional) The attribute used in the path (see {@link Ext.data.Node#getPath} for more info)
     * @param {Function} callback (optional) The callback to call when the expand is complete. The callback will be called with
     * (bSuccess, oLastNode) where bSuccess is if the expand was successful and oLastNode is the last node that was expanded.
     */
    expandPath : function(path, attr, callback){
        var keys = path.split(this.pathSeparator);
        var curNode = this.root;
        var curPath = curNode.attributes.path;
        var index = 1;
        var f = function(){
            if(++index == keys.length){
                if(callback){
                    callback(true, curNode);
                }
                return;
            }
            
            if (index > 2) {
                var c = curNode.findChild('path', curPath + '/' + keys[index]);
            } else {
                var c = curNode.findChild('id', keys[index]);
            }
            if(!c){
                if(callback){
                    callback(false, curNode);
                }
                return;
            }
            curNode = c;
            curPath = c.attributes.path;
            c.expand(false, false, f);
        };
        curNode.expand(false, false, f);
    },
    
    /**
     * files/folder got dropped on node
     * 
     * @param {Object} dropEvent
     * @private
     */
    onBeforeNodeDrop: function(dropEvent) {
        var nodes, target = dropEvent.target;
        
        if(dropEvent.data.selections) {
            nodes = dropEvent.data.grid.selModel.selections.items;
        }    
            
        if(!nodes && dropEvent.data.node) {
            nodes = [dropEvent.data.node];
        }
        
        Tine.Filemanager.fileRecordBackend.copyNodes(nodes, target, !dropEvent.rawEvent.ctrlKey);
        
        dropEvent.dropStatus = true;
        return true;
    },
    
    /**
     * folder delete handler
     */
    onFolderDelete: function(node) {
        var grid = this.app.getMainScreen().getCenterPanel();
        if(grid.currentFolderNode.isAncestor && typeof grid.currentFolderNode.isAncestor == 'function' 
            && grid.currentFolderNode.isAncestor(node)) {
            node.parentNode.select();
        }
        grid.getStore().reload();
    },
    
    /**
     * clone a tree node / create a node from grid node
     * 
     * @param node
     * @returns {Ext.tree.AsyncTreeNode}
     */
    cloneTreeNode: function(node, target) {
        var targetPath = target.attributes.path,
            newPath = '',
            copy;
        
        if(node.attributes) {
            var nodeName = node.attributes.name;
            if(typeof nodeName == 'object') {
                nodeName = nodeName.name;
            }
            newPath = targetPath + '/' + nodeName;
            
            copy = new Ext.tree.AsyncTreeNode({text: node.text, path: newPath, name: node.attributes.name
                , nodeRecord: node.attributes.nodeRecord, account_grants: node.attributes.account_grants});
        }
        else {
            var nodeName = node.data.name;
            if(typeof nodeName == 'object') {
                nodeName = nodeName.name;
            }
            
            var nodeData = Ext.copyTo({}, node.data, Tine.Filemanager.Model.Node.getFieldNames());
            var newNodeRecord = new Tine.Filemanager.Model.Node(nodeData);
             
            newPath = targetPath + '/' + nodeName;
            copy = new Ext.tree.AsyncTreeNode({text: nodeName, path: newPath, name: node.data.name
                , nodeRecord: newNodeRecord, account_grants: node.data.account_grants});
        }
                
        copy.attributes.nodeRecord.beginEdit();
        copy.attributes.nodeRecord.set('path', newPath);
        copy.attributes.nodeRecord.endEdit();
        
        copy.parentNode = target;
        return copy;
    },
    
    /**
     * create Tree node by given node data
     * 
     * @param nodeData
     * @param target
     * @returns {Ext.tree.AsyncTreeNode}
     */
    createTreeNode: function(nodeData, target) {
        var nodeName = nodeData.name;
        if(typeof nodeName == 'object') {
            nodeName = nodeName.name;
        }
        
        var newNodeRecord = new Tine.Filemanager.Model.Node(nodeData);
        
        var newNode = new Ext.tree.AsyncTreeNode({
            text: nodeName,
            path: nodeData.path,
            name: nodeData.name,
            nodeRecord: newNodeRecord,
            account_grants: nodeData.account_grants,
            id: nodeData.id
        })
        
        newNode.attributes.nodeRecord.beginEdit();
        newNode.attributes.nodeRecord.set('path', nodeData.path);
        newNode.attributes.nodeRecord.endEdit();
        
        newNode.parentNode = target;
        return newNode;
        
    },
    
    /**
     * TODO: move to Upload class or elsewhere??
     * updating fileRecord after creating node
     * 
     * @param response
     * @param request
     * @param upload
     */
    onNodeCreated: function(response, request, upload) {
       
        var app = Tine.Tinebase.appMgr.get('Filemanager'),
        grid = app.getMainScreen().getCenterPanel();
        
        var record = Ext.util.JSON.decode(response.responseText);
        
        var fileRecord = upload.fileRecord;
        fileRecord.beginEdit();
        fileRecord.set('contenttype', record.contenttype);
        fileRecord.set('created_by', Tine.Tinebase.registry.get('currentAccount'));
        fileRecord.set('creation_time', record.creation_time);
        fileRecord.set('revision', record.revision);
        fileRecord.set('last_modified_by', record.last_modified_by);
        fileRecord.set('last_modified_time', record.last_modified_time);
        fileRecord.set('status', 'complete');
        fileRecord.set('progress', 100);
        fileRecord.set('name', record.name);
        fileRecord.set('path', record.path);
        fileRecord.commit(false);
        
        upload.fireEvent('update', 'uploadfinished', upload, fileRecord);
        
        grid.pagingToolbar.refresh.enable();
        
    },
    
    /**
     * copies uploaded temporary file to target location
     * 
     * @param upload    {Ext.ux.file.Upload}
     * @param file  {Ext.ux.file.Upload.file} 
     */
    onUploadComplete: function(upload, file) {
        var app = Tine.Tinebase.appMgr.get('Filemanager'),
            treePanel = app.getMainScreen().getWestPanel().getContainerTreePanel();
        
     // check if we are responsible for the upload
        if (upload.fmDirector != treePanel) return;
        
        // $filename, $type, $tempFileId, $forceOverwrite
        Ext.Ajax.request({
            timeout: 10*60*1000, // Overriding Ajax timeout - important!
            params: {
                method: 'Filemanager.createNode',
                filename: upload.id,
                type: 'file',
                tempFileId: file.get('id'),
                forceOverwrite: true
            },
            success: treePanel.onNodeCreated.createDelegate(this, [upload], true), 
            failure: treePanel.onNodeCreated.createDelegate(this, [upload], true)
        });
        
    },
    
    /**
     * on upload failure
     * 
     * @private
     */
    onUploadFail: function () {
        Ext.MessageBox.alert(
            _('Upload Failed'), 
            _('Could not upload file. Filesize could be too big. Please notify your Administrator. Max upload size: ') + Tine.Tinebase.registry.get('maxFileUploadSize')
        ).setIcon(Ext.MessageBox.ERROR);
    },
    
    /**
     * add folder handler
     */
    onFolderAdd: function(nodeData) {
        
        var app = Tine.Tinebase.appMgr.get('Filemanager'),
            grid = app.getMainScreen().getCenterPanel();
        
        grid.getStore().reload();
        if(nodeData.error) {
            Tine.log.debug(nodeData);
        }
    },
    
    /**
     * handles renaming of a tree node / aka folder
     */
    onFolderRename: function(nodeData, node, newName) {
        var app = Tine.Tinebase.appMgr.get('Filemanager'),
            grid = app.getMainScreen().getCenterPanel();
        
        if(nodeData[0]) {
            nodeData = nodeData[0];
        };
        
        node.attributes.nodeRecord.beginEdit();
        node.attributes.nodeRecord.set('name', newName);
        node.attributes.nodeRecord.set('path', nodeData.path);
        node.attributes.path = nodeData.path;
        node.attributes.nodeRecord.commit(false);
        
        if(typeof node.attributes.name == 'object') {
            node.attributes.name.name = newName;
        }
        else {
            node.attributes.name = newName;
        }
        
        grid.currenFolderNode = node;
        
        Tine.Filemanager.NodeTreePanel.superclass.onSelectionChange.call(this, this.getSelectionModel(), node);
        
    },
    
    /**
     * upload update handler
     * 
     * @param change {String} kind of change
     * @param upload {Ext.ux.file.Upload} upload
     * @param fileRecord {file} fileRecord
     * 
     */
    onUpdate: function(change, upload, fileRecord) {
        
        var app = Tine.Tinebase.appMgr.get('Filemanager'),
            grid = app.getMainScreen().getCenterPanel(),
            treePanel = app.getMainScreen().getWestPanel().getContainerTreePanel(),
            rowsToUpdate = grid.getStore().query('name', fileRecord.get('name'));
        
        if(change == 'uploadstart') {
            Tine.Tinebase.uploadManager.onUploadStart();
        }
        else if(change == 'uploadfailure') {
            treePanel.onUploadFail();
        }
        
        if(rowsToUpdate.get(0)) {
            if(change == 'uploadcomplete') {
                treePanel.onUploadComplete(upload, fileRecord);
            }
            else if(change == 'uploadfinished') {
                rowsToUpdate.get(0).set('size', upload.fileSize);
                rowsToUpdate.get(0).set('contenttype', fileRecord.get('contenttype'));
            }
            rowsToUpdate.get(0).afterEdit();
            rowsToUpdate.get(0).commit(false);
        }       
    },
    
    /**
     * handels tree drop of object from outside the browser
     * 
     * @param fileSelector
     * @param targetNodeId
     */
    dropIntoTree: function(fileSelector, event) {
              
        var treePanel = fileSelector.component,
            app = treePanel.app,
            grid = app.getMainScreen().getCenterPanel(),
            targetNode,
            targetNodePath;
            
        
        var targetNodeId;
        var treeNodeAttribute = event.getTarget('div').attributes['ext:tree-node-id'];
        if(treeNodeAttribute) {
            targetNodeId = treeNodeAttribute.nodeValue;
            targetNode = treePanel.getNodeById(targetNodeId);
            targetNodePath = targetNode.attributes.path;
            
        };
        
        if(!targetNode.attributes.nodeRecord.isDropFilesAllowed()) {
            Ext.MessageBox.alert(
                    _('Upload Failed'), 
                    app.i18n._('Putting files in this folder is not allowed!')
                ).setIcon(Ext.MessageBox.ERROR);
            
            return;
        };
      
        var files = fileSelector.getFileList(),
            filePathsArray = [],
            uploadKeyArray = [],
            addToGridStore = false;
        
        Ext.each(files, function (file) {
            
            var fileName = file.name || file.fileName,
                filePath = targetNodePath + '/' + fileName;
            
            var upload = new Ext.ux.file.Upload({
                fmDirector: treePanel,
                file: file,
                fileSelector: fileSelector,
                id: filePath
            });
            
            var uploadKey = Tine.Tinebase.uploadManager.queueUpload(upload);
            
            filePathsArray.push(filePath);
            uploadKeyArray.push(uploadKey);
            
            addToGridStore = grid.currentFolderNode.id === targetNodeId;
            
        }, this);
        
        var params = {
                filenames: filePathsArray,
                type: "file",
                tempFileIds: [],
                forceOverwrite: false
        };
        Tine.Filemanager.fileRecordBackend.createNodes(params, uploadKeyArray, addToGridStore);
    }
});
/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Martin Jatho <m.jatho@metaways.de>
 * @copyright   Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 */
Ext.ns('Tine.Filemanager');

/**
 * File grid panel
 * 
 * @namespace   Tine.Filemanager
 * @class       Tine.Filemanager.NodeGridPanel
 * @extends     Tine.widgets.grid.GridPanel
 * 
 * <p>Node Grid Panel</p>
 * <p><pre>
 * </pre></p>
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Martin Jatho <m.jatho@metaways.de>
 * @copyright   Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Filemanager.FileGridPanel
 */
Tine.Filemanager.NodeGridPanel = Ext.extend(Tine.widgets.grid.GridPanel, {   
    /**
     * @cfg filesProperty
     * @type String
     */
    filesProperty: 'files',
    
    /**
     * @cfg showTopToolbar
     * @type Boolean
     * TODO     think about that -> when we deactivate the top toolbar, we lose the dropzone for files!
     */
    //showTopToolbar: null,
    
    /**
     * config values
     * @private
     */
    header: false,
    border: false,
    deferredRender: false,
    autoExpandColumn: 'name',
    showProgress: true,
    
    recordClass: Tine.Filemanager.Model.Node,
    hasDetailsPanel: false,
    evalGrants: true,
    
    /**
     * grid specific
     * @private
     */
    defaultSortInfo: {field: 'name', direction: 'DESC'},
    gridConfig: {
        autoExpandColumn: 'name',
        enableFileDialog: false,
        enableDragDrop: true,
        ddGroup: 'fileDDGroup'
    },
     
    ddGroup : 'fileDDGroup',  
    currentFolderNode : '/',
    
    /**
     * inits this cmp
     * @private
     */
    initComponent: function() {
        this.recordProxy = Tine.Filemanager.fileRecordBackend;
        
        this.gridConfig.cm = this.getColumnModel();
        
        this.defaultFilters = [
            {field: 'query', operator: 'contains', value: ''},
            {field: 'path', operator: 'equals', value: '/'}
        ];
        
        this.filterToolbar = this.filterToolbar || this.getFilterToolbar();
        this.filterToolbar.getQuickFilterPlugin().criteriaIgnores.push({field: 'path'});
        
        this.plugins = this.plugins || [];
        this.plugins.push(this.filterToolbar);
        this.plugins.push({
            ptype: 'ux.browseplugin',
            multiple: true,
            scope: this,
            enableFileDialog: false,
            handler: this.onFilesSelect
        });
        
        Tine.Filemanager.NodeGridPanel.superclass.initComponent.call(this);
        this.getStore().on('load', this.onLoad);
        Tine.Tinebase.uploadManager.on('update', this.onUpdate);
    },
    
    /**
     * after render handler
     */
    afterRender: function() {
        Tine.Filemanager.NodeGridPanel.superclass.afterRender.call(this);
        this.action_upload.setDisabled(true);
        this.initDropTarget();
        this.currentFolderNode = this.app.getMainScreen().getWestPanel().getContainerTreePanel().getRootNode();
    },
    
    /**
     * returns cm
     * 
     * @return Ext.grid.ColumnModel
     * @private
     * 
     * TODO    add more columns
     */
    getColumnModel: function(){
        var columns = [{ 
                id: 'tags',
                header: this.app.i18n._('Tags'),
                dataIndex: 'tags',
                width: 50,
                renderer: Tine.Tinebase.common.tagsRenderer,
                sortable: false,
                hidden: false
            }, {
                id: 'name',
                header: this.app.i18n._("Name"),
                width: 70,
                sortable: true,
                dataIndex: 'name',
                renderer: Ext.ux.PercentRendererWithName
            },{
                id: 'size',
                header: this.app.i18n._("Size"),
                width: 40,
                sortable: true,
                dataIndex: 'size',
                renderer: Tine.Tinebase.common.byteRenderer
            },{
                id: 'contenttype',
                header: this.app.i18n._("Contenttype"),
                width: 50,
                sortable: true,
                dataIndex: 'contenttype',
                renderer: function(value, metadata, record) {
                    
                    var app = Tine.Tinebase.appMgr.get('Filemanager');
                    if(record.data.type == 'folder') {
                        return app.i18n._("Folder");
                    }
                    else {
                        return value;
                    }
                }
            },
//            {
//                id: 'revision',
//                header: this.app.i18n._("Revision"),
//                width: 10,
//                sortable: true,
//                dataIndex: 'revision',
//                renderer: function(value, metadata, record) {
//                    if(record.data.type == 'folder') {
//                        return '';
//                    }
//                    else {
//                        return value;
//                    }
//                }
//            },
            {
                id: 'creation_time',
                header: this.app.i18n._("Creation Time"),
                width: 50,
                sortable: true,
                dataIndex: 'creation_time',
                renderer: Tine.Tinebase.common.dateTimeRenderer
                
            },{
                id: 'created_by',
                header: this.app.i18n._("Created By"),
                width: 50,
                sortable: true,
                dataIndex: 'created_by',
                renderer: Tine.Tinebase.common.usernameRenderer
            },{
                id: 'last_modified_time',
                header: this.app.i18n._("Last Modified Time"),
                width: 80,
                sortable: true,
                dataIndex: 'last_modified_time',
                renderer: Tine.Tinebase.common.dateTimeRenderer
            },{
                id: 'last_modified_by',
                header: this.app.i18n._("Last Modified By"),
                width: 50,
                sortable: true,
                dataIndex: 'last_modified_by',
                renderer: Tine.Tinebase.common.usernameRenderer 
            }
        ];
        
        return new Ext.grid.ColumnModel({
            defaults: {
                sortable: true,
                resizable: true
            },
            columns: columns
        });
    },
    
    /**
     * status column renderer
     * @param {string} value
     * @return {string}
     */
    statusRenderer: function(value) {
        return this.app.i18n._hidden(value);
    },
    
    /**
     * init ext grid panel
     * @private
     */
    initGrid: function() {
        Tine.Filemanager.NodeGridPanel.superclass.initGrid.call(this);
        
        if (this.usePagingToolbar) {
           this.initPagingToolbar();
        }
    },
    
    /**
     * inserts a quota Message when using old Browsers with html4upload
     */
    initPagingToolbar: function() {
        if(!this.pagingToolbar || !this.pagingToolbar.rendered) {
            this.initPagingToolbar.defer(50, this);
            return;
        }
        // old browsers
        if (!((! Ext.isGecko && window.XMLHttpRequest && window.File && window.FileList) || (Ext.isGecko && window.FileReader))) {
            var text = new Ext.Panel({padding: 2, html: String.format(this.app.i18n._('The max. Upload Filesize is {0} MB'), Tine.Tinebase.registry.get('maxFileUploadSize') / 1048576 )});
            this.pagingToolbar.insert(12, new Ext.Toolbar.Separator());
            this.pagingToolbar.insert(12, text);
            this.pagingToolbar.doLayout();
        }
    },
    
    /**
     * returns filter toolbar -> supress OR filters
     * @private
     */
    getFilterToolbar: function(config) {
        config = config || {};
        var plugins = [];
        if (! Ext.isDefined(this.hasQuickSearchFilterToolbarPlugin) || this.hasQuickSearchFilterToolbarPlugin) {
            this.quickSearchFilterToolbarPlugin = new Tine.widgets.grid.FilterToolbarQuickFilterPlugin();
            plugins.push(this.quickSearchFilterToolbarPlugin);
        }
        
        return new Tine.widgets.grid.FilterToolbar(Ext.apply(config, {
            app: this.app,
            recordClass: this.recordClass,
            filterModels: this.recordClass.getFilterModel().concat(this.getCustomfieldFilters()),
            defaultFilter: 'query',
            filters: this.defaultFilters || [],
            plugins: plugins
        }));
    },
    
    /**
     * returns add action / test
     * 
     * @return {Object} add action config
     */
    getAddAction: function () {
        return {
            requiredGrant: 'addGrant',
            actionType: 'add',
            text: this.app.i18n._('Upload'),
            handler: this.onFilesSelect,
            disabled: true,
            scope: this,
            plugins: [{
                ptype: 'ux.browseplugin',
                multiple: true,
                enableFileDrop: false,
                disable: true
            }],
            iconCls: this.app.appName + 'IconCls'
        };
    },
    
    /**
     * init actions with actionToolbar, contextMenu and actionUpdater
     * @private
     */
    initActions: function() {
        this.action_upload = new Ext.Action(this.getAddAction());
        
        this.action_editFile = new Ext.Action({
            requiredGrant: 'editGrant',
            allowMultiple: false,
            text: this.app.i18n._('Edit Properties'),
            handler: this.onEditFile,
            iconCls: 'action_edit_file',
            disabled: false,
            actionType: 'edit',
            scope: this
        });
        this.action_createFolder = new Ext.Action({
            requiredGrant: 'addGrant',
            actionType: 'reply',
            allowMultiple: true,
            text: this.app.i18n._('Create Folder'),
            handler: this.onCreateFolder,
            iconCls: 'action_create_folder',
            disabled: true,
            scope: this
        });
        
        this.action_goUpFolder = new Ext.Action({
//            requiredGrant: 'readGrant',
            allowMultiple: true,
            actionType: 'goUpFolder',
            text: this.app.i18n._('Folder Up'),
            handler: this.onLoadParentFolder,
            iconCls: 'action_filemanager_folder_up',
            disabled: true,
            scope: this
        });
        
        this.action_download = new Ext.Action({
            requiredGrant: 'readGrant',
            allowMultiple: false,
            actionType: 'download',
            text: this.app.i18n._('Save locally'),
            handler: this.onDownload,
            iconCls: 'action_filemanager_save_all',
            disabled: true,
            scope: this
        });
        
        this.action_deleteRecord = new Ext.Action({
            requiredGrant: 'deleteGrant',
            allowMultiple: true,
            singularText: this.app.i18n._('Delete'),
            pluralText: this.app.i18n._('Delete'),
            translationObject: this.i18nDeleteActionText ? this.app.i18n : Tine.Tinebase.translation,
            text: this.app.i18n._('Delete'),
            handler: this.onDeleteRecords,
            disabled: true,
            iconCls: 'action_delete',
            scope: this
        });
        
        this.contextMenu = Tine.Filemanager.GridContextMenu.getMenu({
            nodeName: Tine.Filemanager.Model.Node.getRecordName(),
            actions: ['delete', 'rename', 'download', 'resume', 'pause', 'edit'],
            scope: this,
            backend: 'Filemanager',
            backendModel: 'Node'
        });
        
        this.folderContextMenu = Tine.Filemanager.GridContextMenu.getMenu({
            nodeName: this.app.i18n._(this.app.getMainScreen().getWestPanel().getContainerTreePanel().containerName),
            actions: ['delete', 'rename'],
            scope: this,
            backend: 'Filemanager',
            backendModel: 'Node'
        });
        
        this.actionUpdater.addActions(this.contextMenu.items);
        this.actionUpdater.addActions(this.folderContextMenu.items);
        
        this.actionUpdater.addActions([
           this.action_createFolder,
           this.action_goUpFolder,
           this.action_download,
           this.action_deleteRecord,
           this.action_editFile
       ]);
    },
    
    /**
     * get the right contextMenu
     */
    getContextMenu: function(grid, row, e) {
        var r = this.store.getAt(row),
            type = r ? r.get('type') : null;
            
        return type === 'folder' ? this.folderContextMenu : this.contextMenu;
    },
    
    /**
     * get action toolbar
     * 
     * @return {Ext.Toolbar}
     */
    getActionToolbar: function() {
        if (! this.actionToolbar) {
            this.actionToolbar = new Ext.Toolbar({
                defaults: {height: 55},
                items: [{
                    xtype: 'buttongroup',
                    columns: 8,
                    defaults: {minWidth: 60},
                    items: [
                        this.splitAddButton ? 
                        Ext.apply(new Ext.SplitButton(this.action_upload), {
                            scale: 'medium',
                            rowspan: 2,
                            iconAlign: 'top',
                            arrowAlign:'right',
                            menu: new Ext.menu.Menu({
                                items: [],
                                plugins: [{
                                    ptype: 'ux.itemregistry',
                                    key:   'Tine.widgets.grid.GridPanel.addButton'
                                }]
                            })
                        }) : 
                        Ext.apply(new Ext.Button(this.action_upload), {
                            scale: 'medium',
                            rowspan: 2,
                            iconAlign: 'top'
                        }),
                        
                        Ext.apply(new Ext.Button(this.action_editFile), {
                            scale: 'medium',
                            rowspan: 2,
                            iconAlign: 'top'
                        }),
                        Ext.apply(new Ext.Button(this.action_deleteRecord), {
                            scale: 'medium',
                            rowspan: 2,
                            iconAlign: 'top'
                        }),
                        Ext.apply(new Ext.Button(this.action_createFolder), {
                            scale: 'medium',
                            rowspan: 2,
                            iconAlign: 'top'
                        }),
                        Ext.apply(new Ext.Button(this.action_goUpFolder), {
                            scale: 'medium',
                            rowspan: 2,
                            iconAlign: 'top'
                        }),
                        Ext.apply(new Ext.Button(this.action_download), {
                            scale: 'medium',
                            rowspan: 2,
                            iconAlign: 'top'
                        })
                 ]
                }, this.getActionToolbarItems()]
            });
            
            if (this.filterToolbar && typeof this.filterToolbar.getQuickFilterField == 'function') {
                this.actionToolbar.add('->', this.filterToolbar.getQuickFilterField());
            }
        }
        
        return this.actionToolbar;
    },
    
    /**
     * opens the edit dialog
     */
    onEditFile: function() {
        var sel = this.getGrid().getSelectionModel().getSelections();

        if(sel.length == 1) {
            var record = new Tine.Filemanager.Model.Node(sel[0].data);
            var window = Tine.Filemanager.NodeEditDialog.openWindow({record: record});
        }
        
        window.on('saveAndClose', function() {
            this.getGrid().store.reload();
        }, this);
    },
    
    /**
     * create folder in current position
     * 
     * @param {Ext.Component} button
     * @param {Ext.EventObject} event
     */
    onCreateFolder: function(button, event) {
        var app = this.app,
            nodeName = Tine.Filemanager.Model.Node.getContainerName();
        
        Ext.MessageBox.prompt(_('New Folder'), _('Please enter the name of the new folder:'), function(_btn, _text) {
            var currentFolderNode = app.getMainScreen().getCenterPanel().currentFolderNode;
            if(currentFolderNode && _btn == 'ok') {
                if (! _text) {
                    Ext.Msg.alert(String.format(_('No {0} added'), nodeName), String.format(_('You have to supply a {0} name!'), nodeName));
                    return;
                }
                
                var filename = currentFolderNode.attributes.path + '/' + _text;
                Tine.Filemanager.fileRecordBackend.createFolder(filename);
                
            }
        }, this);  
    },
    
    /**
     * delete selected files / folders
     * 
     * @param {Ext.Component} button
     * @param {Ext.EventObject} event
     */
    onDeleteRecords: function(button, event) {
        var app = this.app,
            nodeName = '',
            sm = app.getMainScreen().getCenterPanel().selectionModel,
            nodes = sm.getSelections();
        
        if(nodes && nodes.length) {
            for(var i=0; i<nodes.length; i++) {
                var currNodeData = nodes[i].data;
                
                if(typeof currNodeData.name == 'object') {
                    nodeName += currNodeData.name.name + '<br />';
                }
                else {
                    nodeName += currNodeData.name + '<br />';
                }
            }
        }
        
        this.conflictConfirmWin = Tine.widgets.dialog.FileListDialog.openWindow({
            modal: true,
            allowCancel: false,
            height: 180,
            width: 300,
            title: app.i18n._('Do you really want to delete the following files?'),
            text: nodeName,
            scope: this,
            handler: function(button){
                if (nodes && button == 'yes') {
                    this.store.remove(nodes);
                    this.pagingToolbar.refresh.disable();
                    Tine.Filemanager.fileRecordBackend.deleteItems(nodes);
                }
                
                for(var i=0; i<nodes.length; i++) {
                    var node = nodes[i];
                    
                    if(node.fileRecord) {
                        var upload = Tine.Tinebase.uploadManager.getUpload(node.fileRecord.get('uploadKey'));
                        upload.setPaused(true);
                        Tine.Tinebase.uploadManager.unregisterUpload(upload.id);
                    }
                    
                }
            }
        }, this);
    },
    
    /**
     * go up one folder
     * 
     * @param {Ext.Component} button
     * @param {Ext.EventObject} event
     */
    onLoadParentFolder: function(button, event) {
        var app = this.app,
            currentFolderNode = app.getMainScreen().getCenterPanel().currentFolderNode;
        
        if(currentFolderNode && currentFolderNode.parentNode) {
            app.getMainScreen().getCenterPanel().currentFolderNode = currentFolderNode.parentNode;
            currentFolderNode.parentNode.select();
        }
    },
    
    /**
     * grid row doubleclick handler
     * 
     * @param {Tine.Filemanager.NodeGridPanel} grid
     * @param {} row record
     * @param {Ext.EventObjet} e
     */
    onRowDblClick: function(grid, row, e) {
        var app = this.app;
        var rowRecord = grid.getStore().getAt(row);
        
        if(rowRecord.data.type == 'file') {
            var downloadPath = rowRecord.data.path;
            var downloader = new Ext.ux.file.Download({
                params: {
                    method: 'Filemanager.downloadFile',
                    requestType: 'HTTP',
                    id: '',
                    path: downloadPath
                }
            }).start();
        }
        
        else if (rowRecord.data.type == 'folder'){
            var treePanel = app.getMainScreen().getWestPanel().getContainerTreePanel();
            
            var currentFolderNode;
            if(rowRecord.data.path == '/personal/system') {
                currentFolderNode = treePanel.getNodeById('personal');
            }
            else if(rowRecord.data.path == '/shared') {
                currentFolderNode = treePanel.getNodeById('shared');
            }
            else if(rowRecord.data.path == '/personal') {
                currentFolderNode = treePanel.getNodeById('otherUsers');
            }
            else {
                currentFolderNode = treePanel.getNodeById(rowRecord.id);
            }
            if(currentFolderNode) {
                currentFolderNode.select();
                currentFolderNode.expand();
                app.getMainScreen().getCenterPanel().currentFolderNode = currentFolderNode;
            } else {
                // get ftb path filter
                this.filterToolbar.filterStore.each(function(filter) {
                    var field = filter.get('field');
                    if (field === 'path') {
                        filter.set('value', '');
                        filter.set('value', rowRecord.data);
                        filter.formFields.value.setValue(rowRecord.get('path'));
                        
                        this.filterToolbar.onFiltertrigger();
                        return false;
                    }
                }, this);
            }
        }
    }, 
        
    /**
     * on upload failure
     * 
     * @private
     */
    onUploadFail: function () {
        Ext.MessageBox.alert(
            _('Upload Failed'), 
            _('Could not upload file. Filesize could be too big. Please notify your Administrator. Max upload size: ') 
            + Tine.Tinebase.common.byteRenderer(Tine.Tinebase.registry.get('maxFileUploadSize')) 
        ).setIcon(Ext.MessageBox.ERROR);
        
        var app = Tine.Tinebase.appMgr.get('Filemanager'),
            grid = app.getMainScreen().getCenterPanel();
        grid.pagingToolbar.refresh.enable();
    },
    
    /**
     * on remove handler
     * 
     * @param {} button
     * @param {} event
     */
    onRemove: function (button, event) {
        var selectedRows = this.selectionModel.getSelections();
        for (var i = 0; i < selectedRows.length; i += 1) {
            this.store.remove(selectedRows[i]);
            var upload = Tine.Tinebase.uploadManager.getUpload(selectedRows[i].get('uploadKey'));
            upload.setPaused(true);
        }
    },
    
    /**
     * populate grid store
     * 
     * @param {} record
     */
    loadRecord: function (record) {
        if (record && record.get(this.filesProperty)) {
            var files = record.get(this.filesProperty);
            for (var i = 0; i < files.length; i += 1) {
                var file = new Ext.ux.file.Upload.file(files[i]);
                file.set('status', 'complete');
                file.set('nodeRecord', new Tine.Filemanager.Model.Node(file.data));
                this.store.add(file);
            }
        }
    },
    
    /**
     * copies uploaded temporary file to target location
     * 
     * @param upload  {Ext.ux.file.Upload}
     * @param file  {Ext.ux.file.Upload.file} 
     */
    onUploadComplete: function(upload, file) {
        var app = Tine.Tinebase.appMgr.get('Filemanager'),
            grid = app.getMainScreen().getCenterPanel();
        
        // check if we are responsible for the upload
        if (upload.fmDirector != grid) return;
        
        // $filename, $type, $tempFileId, $forceOverwrite
        Ext.Ajax.request({
            timeout: 10*60*1000, // Overriding Ajax timeout - important!
            params: {
                method: 'Filemanager.createNode',
                filename: upload.id,
                type: 'file',
                tempFileId: file.get('id'),
                forceOverwrite: true
            },
            success: grid.onNodeCreated.createDelegate(this, [upload], true), 
            failure: grid.onNodeCreated.createDelegate(this, [upload], true)
        });
        
    },
    
    /**
     * TODO: move to Upload class or elsewhere??
     * updating fileRecord after creating node
     * 
     * @param response
     * @param request
     * @param upload
     */
    onNodeCreated: function(response, request, upload) {
        var record = Ext.util.JSON.decode(response.responseText);
                
        var fileRecord = upload.fileRecord;
        fileRecord.beginEdit();
        fileRecord.set('contenttype', record.contenttype);
        fileRecord.set('created_by', Tine.Tinebase.registry.get('currentAccount'));
        fileRecord.set('creation_time', record.creation_time);
        fileRecord.set('revision', record.revision);
        fileRecord.set('last_modified_by', record.last_modified_by);
        fileRecord.set('last_modified_time', record.last_modified_time);
        fileRecord.set('name', record.name);
        fileRecord.set('path', record.path);
        fileRecord.set('status', 'complete');
        fileRecord.set('progress', 100);
        fileRecord.commit(false);
       
        upload.fireEvent('update', 'uploadfinished', upload, fileRecord);
        
        var app = Tine.Tinebase.appMgr.get('Filemanager'),
            grid = app.getMainScreen().getCenterPanel();
        
        var allRecordsComplete = true;
        var storeItems = grid.getStore().getRange();
        for(var i=0; i<storeItems.length; i++) {
            if(storeItems[i].get('status') && storeItems[i].get('status') !== 'complete') {
                allRecordsComplete = false;
                break;
            }
        }
        
        if(allRecordsComplete) {
            grid.pagingToolbar.refresh.enable();
        }
    },
    
    /**
     * upload new file and add to store
     * 
     * @param {ux.BrowsePlugin} fileSelector
     * @param {} e
     */
    onFilesSelect: function (fileSelector, event) {
        var app = Tine.Tinebase.appMgr.get('Filemanager'),
            grid = app.getMainScreen().getCenterPanel(),
            targetNode = grid.currentFolderNode,
            gridStore = grid.store,
            rowIndex = false,
            targetFolderPath = grid.currentFolderNode.attributes.path,
            addToGrid = true,
            dropAllowed = false,
            nodeRecord = null;
        
        if(event && event.getTarget()) {
            rowIndex = grid.getView().findRowIndex(event.getTarget());
        }
        
        
        if(targetNode.attributes) {
            nodeRecord = targetNode.attributes.nodeRecord;
        }
        
        if(rowIndex !== false && rowIndex > -1) {
            var newTargetNode = gridStore.getAt(rowIndex);
            if(newTargetNode && newTargetNode.data.type == 'folder') {
                targetFolderPath = newTargetNode.data.path;
                addToGrid = false;
                nodeRecord = new Tine.Filemanager.Model.Node(newTargetNode.data);
            }
        }
        
        if(!nodeRecord.isDropFilesAllowed()) {
            Ext.MessageBox.alert(
                    _('Upload Failed'), 
                    app.i18n._('Putting files in this folder is not allowed!')
            ).setIcon(Ext.MessageBox.ERROR);
            
            return;
        }    
        
        var files = fileSelector.getFileList();
        
        if(files.length > 0) {
            grid.pagingToolbar.refresh.disable();
        }
        
        var filePathsArray = [], uploadKeyArray = [];
        
        Ext.each(files, function (file) {
            var fileRecord = Tine.Filemanager.Model.Node.createFromFile(file),
                filePath = targetFolderPath + '/' + fileRecord.get('name');
            
            fileRecord.set('path', filePath);
            var existingRecordIdx = gridStore.find('name', fileRecord.get('name'));
            if(existingRecordIdx < 0) {
                gridStore.add(fileRecord);
            }
            
            var upload = new Ext.ux.file.Upload({
                fmDirector: grid,
                file: file,
                fileSelector: fileSelector,
                id: filePath
            });
            
            var uploadKey = Tine.Tinebase.uploadManager.queueUpload(upload);
            
            filePathsArray.push(filePath);
            uploadKeyArray.push(uploadKey);
            
        }, this);
        
        var params = {
                filenames: filePathsArray,
                type: "file",
                tempFileIds: [],
                forceOverwrite: false
        };
        Tine.Filemanager.fileRecordBackend.createNodes(params, uploadKeyArray, true);
    },
    
    /**
     * download file
     * 
     * @param {} button
     * @param {} event
     */
    onDownload: function(button, event) {
        
        var app = Tine.Tinebase.appMgr.get('Filemanager'),
            grid = app.getMainScreen().getCenterPanel(),
            selectedRows = grid.selectionModel.getSelections();
        
        var fileRow = selectedRows[0];
               
        var downloadPath = fileRow.data.path;
        var downloader = new Ext.ux.file.Download({
            params: {
                method: 'Filemanager.downloadFile',
                requestType: 'HTTP',
                id: '',
                path: downloadPath
            }
        }).start();
    },
    
    /**
     * grid on load handler
     * 
     * @param grid
     * @param records
     * @param options
     */
    onLoad: function(store, records, options){
        var app = Tine.Tinebase.appMgr.get('Filemanager'),
            grid = app.getMainScreen().getCenterPanel();
        
        for(var i=records.length-1; i>=0; i--) {
            var record = records[i];
            if(record.get('type') == 'file' && (!record.get('size') || record.get('size') == 0)) {
                var upload = Tine.Tinebase.uploadManager.getUpload(record.get('path'));
                
                if(upload) {
                      if(upload.fileRecord && record.get('name') == upload.fileRecord.get('name')) {
                          grid.updateNodeRecord(record, upload.fileRecord);
                          record.afterEdit();
                    }
                }
            }
        }
    },
    
    /**
     * update grid nodeRecord with fileRecord data
     * 
     * @param nodeRecord
     * @param fileRecord
     */
    updateNodeRecord: function(nodeRecord, fileRecord) {
        for(var field in fileRecord.fields) {
            nodeRecord.set(field, fileRecord.get(field));
        }
        nodeRecord.fileRecord = fileRecord;
    },
    
    /**
     * upload update handler
     * 
     * @param change {String} kind of change
     * @param upload {Ext.ux.file.Upload} upload
     * @param fileRecord {file} fileRecord
     * 
     */
    onUpdate: function(change, upload, fileRecord) {
        var app = Tine.Tinebase.appMgr.get('Filemanager'),
            grid = app.getMainScreen().getCenterPanel(),
            rowsToUpdate = grid.getStore().query('name', fileRecord.get('name'));
        
        if(change == 'uploadstart') {
            Tine.Tinebase.uploadManager.onUploadStart();
        }
        else if(change == 'uploadfailure') {
            grid.onUploadFail();
        }
        
        if(rowsToUpdate.get(0)) {
            if(change == 'uploadcomplete') {
                grid.onUploadComplete(upload, fileRecord);
            }
            else if(change == 'uploadfinished') {
                rowsToUpdate.get(0).set('size', fileRecord.get('size'));
                rowsToUpdate.get(0).set('contenttype', fileRecord.get('contenttype'));
            }
            rowsToUpdate.get(0).afterEdit();
            rowsToUpdate.get(0).commit(false);
        }
    },
    
    /**
     * init grid drop target
     * 
     * @TODO DRY cleanup
     */
    initDropTarget: function(){
        var ddrow = new Ext.dd.DropTarget(this.getEl(), {
            ddGroup : 'fileDDGroup',  
            
            notifyDrop : function(dragSource, e, data){
                
                if(data.node && data.node.attributes && !data.node.attributes.nodeRecord.isDragable()) {
                    return false;
                }
                
                var app = Tine.Tinebase.appMgr.get(Tine.Filemanager.fileRecordBackend.appName),
                    grid = app.getMainScreen().getCenterPanel(),
                    treePanel = app.getMainScreen().getWestPanel().getContainerTreePanel(),
                    dropIndex = grid.getView().findRowIndex(e.target),
                    target = grid.getStore().getAt(dropIndex),
                    nodes = data.selections ? data.selections : [data.node];
                
                if((!target || target.data.type === 'file') && grid.currentFolderNode) {
                    target = grid.currentFolderNode;
                }
                
                if(!target) {
                    return false;
                }
                
                for(var i=0; i<nodes.length; i++) {
                    if(nodes[i].id == target.id) {
                        return false;
                    }
                }
                
                var targetNode = treePanel.getNodeById(target.id);
                if(targetNode && targetNode.isAncestor(nodes[0])) {
                    return false;
                }
                
                Tine.Filemanager.fileRecordBackend.copyNodes(nodes, target, !e.ctrlKey);
                return true;
            },
            
            notifyOver : function( dragSource, e, data ) {
                if(data.node && data.node.attributes && !data.node.attributes.nodeRecord.isDragable()) {
                    return false;
                }
                
                var app = Tine.Tinebase.appMgr.get(Tine.Filemanager.fileRecordBackend.appName),
                    grid = app.getMainScreen().getCenterPanel(),
                    dropIndex = grid.getView().findRowIndex(e.target),
                    treePanel = app.getMainScreen().getWestPanel().getContainerTreePanel(),
                    target= grid.getStore().getAt(dropIndex),
                    nodes = data.selections ? data.selections : [data.node];
                
                if((!target || (target.data && target.data.type === 'file')) && grid.currentFolderNode) {
                    target = grid.currentFolderNode;
                }
                
                if(!target) {
                    return false;
                }
                
                for(var i=0; i<nodes.length; i++) {
                    if(nodes[i].id == target.id) {
                        return false;
                    }
                }
                
                var targetNode = treePanel.getNodeById(target.id);
                if(targetNode && targetNode.isAncestor(nodes[0])) {
                    return false;
                }
                
                return this.dropAllowed;
            }
        });
    }
});
/*
 * Tine 2.0
 * 
 * @package     Tinebase
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2010-2012 Metaways Infosystems GmbH (http://www.metaways.de)
 */

Ext.ns('Tine.Filemanager');

/**
 * @namespace Tine.Filemanager
 * @class Tine.Filemanager.Application
 * @extends Tine.Tinebase.Application
 */
Tine.Filemanager.Application = Ext.extend(Tine.Tinebase.Application, {
    hasMainScreen : true,

    /**
     * Get translated application title of this application
     * 
     * @return {String}
     */
    getTitle : function() {
        return this.i18n.gettext('Filemanager');
    }
});

/*
 * register additional action for genericpickergridpanel
 */
Tine.widgets.relation.MenuItemManager.register('Filemanager', 'Node', {
    text: 'Save locally',   // _('Save locally')
    iconCls: 'action_filemanager_save_all',
    requiredGrant: 'readGrant',
    actionType: 'download',
    allowMultiple: false,
    handler: function(action) {
        var node = action.grid.store.getAt(action.gridIndex).get('related_record');
        var downloadPath = node.path;
        var downloader = new Ext.ux.file.Download({
            params: {
                method: 'Filemanager.downloadFile',
                requestType: 'HTTP',
                id: '',
                path: downloadPath
            }
        }).start();
    }
});

/**
 * @namespace Tine.Filemanager
 * @class Tine.Filemanager.MainScreen
 * @extends Tine.widgets.MainScreen
 */
Tine.Filemanager.MainScreen = Ext.extend(Tine.widgets.MainScreen, {
    activeContentType: 'Node'
});

/**
 * generic exception handler for filemanager
 * 
 * @param {Tine.Exception} exception
 */
Tine.Filemanager.handleRequestException = function(exception, request) {
    
    var app = Tine.Tinebase.appMgr.get('Filemanager'),
        existingFilenames = [],
        nonExistantFilenames = [],
        i,
        filenameWithoutPath = null;
    
    switch(exception.code) {
        // overwrite default 503 handling and add a link to the wiki
        case 503:
            Ext.MessageBox.show({
                buttons: Ext.Msg.OK,
                icon: Ext.MessageBox.WARNING,
                title: _('Service Unavailable'), 
                msg: String.format(app.i18n._('The Filemanager is not configured correctly. Please refer to the {0}Tine 2.0 Admin FAQ{1} for configuration advice or contact your administrator.'),
                    '<a href="http://www.tine20.org/wiki/index.php/Admin_FAQ#The_message_.22filesdir_config_value_not_set.22_appears_in_the_logfile_and_I_can.27t_open_the_Filemanager" target="_blank">',
                    '</a>')
            });
            break;
            
        case 901: 
            if (request) {
                Tine.log.debug('Tine.Filemanager.handleRequestException - request exception:');
                Tine.log.debug(exception);
                
                if (exception.existingnodesinfo) {
                    for (i = 0; i < exception.existingnodesinfo.length; i++) {
                        existingFilenames.push(exception.existingnodesinfo[i].name);
                    }
                }
                
                this.conflictConfirmWin = Tine.widgets.dialog.FileListDialog.openWindow({
                    modal: true,
                    allowCancel: false,
                    height: 180,
                    width: 300,
                    title: app.i18n._('Files already exists') + '. ' + app.i18n._('Do you want to replace the following file(s)?'),
                    text: existingFilenames.join('<br />'),
                    scope: this,
                    handler: function(button) {
                        var params = request.params,
                            uploadKey = exception.uploadKeyArray;
                        params.method = request.method;
                        params.forceOverwrite = true;
                        
                        if (button == 'no') {
                            Tine.log.debug('Tine.Filemanager.handleRequestException::' + params.method + ' -> only non-existant nodes.');
                            Ext.each(params.filenames, function(filename) {
                                filenameWithoutPath = filename.match(/[^\/]*$/);
                                if (filenameWithoutPath && existingFilenames.indexOf(filenameWithoutPath[0]) === -1) {
                                    nonExistantFilenames.push(filename);
                                }
                            });
                            params.filenames = nonExistantFilenames;
                            uploadKey = nonExistantFilenames;
                        } else {
                            Tine.log.debug('Tine.Filemanager.handleRequestException::' + params.method + ' -> replace all existing nodes.');
                        }
                        
                        if (params.method == 'Filemanager.copyNodes' || params.method == 'Filemanager.moveNodes' ) {
                            Tine.Filemanager.fileRecordBackend.copyNodes(null, null, null, params);
                        } else if (params.method == 'Filemanager.createNodes' ) {
                            Tine.Filemanager.fileRecordBackend.createNodes(params, uploadKey, exception.addToGridStore);
                        }
                    }
                });
                
            } else {
                Ext.Msg.show({
                  title:   app.i18n._('Failure on create folder'),
                  msg:     app.i18n._('Item with this name already exists!'),
                  icon:    Ext.MessageBox.ERROR,
                  buttons: Ext.Msg.OK
               });
            }
            break;
            
        default:
            Tine.Tinebase.ExceptionHandler.handleRequestException(exception);
            break;
    }
};
