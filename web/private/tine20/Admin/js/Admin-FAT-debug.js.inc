/*!
 * Tine 2.0 - Admin 
 * Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 * http://www.gnu.org/licenses/agpl.html AGPL Version 3
 */
/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2007-2010 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 * @todo        refactor this (split file, use new windows, ...)
 */

/*global Ext, Tine, Locale*/

Ext.ns('Tine.Admin');

Tine.Admin = function () {
    
    /**
     * builds the admin applications tree
     */
    var getInitialTree = function (translation) {
        
        return [{
            text: translation.ngettext('User', 'Users', 50),
            cls: 'treemain',
            iconCls: 'admin-node-user',
            allowDrag: false,
            allowDrop: true,
            id: 'accounts',
            icon: false,
            children: [],
            leaf: null,
            expanded: true,
            dataPanelType: 'accounts',
            viewRight: 'accounts'
        }, {
            text: translation.gettext('Groups'),
            cls: 'treemain',
            iconCls: 'admin-node-groups',
            allowDrag: false,
            allowDrop: true,
            id: 'groups',
            icon: false,
            children: [],
            leaf: null,
            expanded: true,
            dataPanelType: 'groups', 
            viewRight: 'accounts'
        }, {
            text: translation.gettext('Roles'),
            cls: "treemain",
            iconCls: 'action_permissions',
            allowDrag: false,
            allowDrop: true,
            id: "roles",
            children: [],
            leaf: null,
            expanded: true,
            dataPanelType: "roles",
            viewRight: 'roles'
        }, {
            text: translation.gettext('Computers'),
            cls: 'treemain',
            iconCls: 'admin-node-computers',
            allowDrag: false,
            allowDrop: true,
            id: 'computers',
            icon: false,
            children: [],
            leaf: null,
            expanded: true,
            dataPanelType: 'computers', 
            viewRight: 'computers'
        }, {
            text: translation.gettext('Applications'),
            cls: "treemain",
            iconCls: 'admin-node-applications',
            allowDrag: false,
            allowDrop: true,
            id: "applications",
            icon: false,
            children: [],
            leaf: null,
            expanded: true,
            dataPanelType: "applications",
            viewRight: 'apps'
        }, {
            text: translation.gettext('Access Log'),
            cls: "treemain",
            iconCls: 'admin-node-accesslog',
            allowDrag: false,
            allowDrop: true,
            id: "accesslog",
            icon: false,
            children: [],
            leaf: null,
            expanded: true,
            dataPanelType: "accesslog",
            viewRight: 'access_log'
        }, {
            text: translation.gettext('Shared Tags'),
            cls: "treemain",
            iconCls: 'action_tag',
            allowDrag: false,
            allowDrop: true,
            id: "sharedtags",
            //icon :false,
            children: [],
            leaf: null,
            expanded: true,
            dataPanelType: "sharedtags",
            viewRight: 'shared_tags'
        }, {
            text: translation.gettext('Containers'),
            cls: "treemain",
            iconCls: 'admin-node-containers',
            allowDrag: false,
            allowDrop: true,
            id: "containers",
            children: [],
            leaf: null,
            expanded: true,
            dataPanelType: "containers",
            viewRight: 'containers'
        }, {
            text: translation.gettext('Customfields'),
            cls: "treemain",
            iconCls: 'admin-node-customfields',
            allowDrag: false,
            allowDrop: true,
            id: "customfields",
            children: [],
            leaf: null,
            expanded: true,
            dataPanelType: "customfields",
            viewRight: 'customfields'
        }, {
            text: translation.gettext('Server Information'),
            cls: "treemain",
            iconCls: 'admin-node-computers',
            allowDrag: false,
            allowDrop: true,
            id: "serverinfo",
            children: [],
            leaf: null,
            expanded: true,
            dataPanelType: "serverinfo"
        }];
    };

    /**
     * creates the admin menu tree
     *
     */
    var getAdminTree = function () {
        
        var translation = new Locale.Gettext();
        translation.textdomain('Admin');
        
        var treeLoader = new Ext.tree.TreeLoader({
            dataUrl: 'index.php',
            baseParams: {
                jsonKey: Tine.Tinebase.registry.get('jsonKey'),
                method: 'Admin.getSubTree',
                location: 'mainTree'
            }
        });
        treeLoader.on("beforeload", function (loader, node) {
            loader.baseParams.node = node.id;
        }, this);
    
        var treePanel = new Ext.tree.TreePanel({
            title: translation.gettext('Admin'),
            id: 'admin-tree',
            iconCls: 'AdminIconCls',
            loader: treeLoader,
            rootVisible: false,
            border: false,
            autoScroll: true
        });
        
        // set the root node
        var treeRoot = new Ext.tree.TreeNode({
            text: 'root',
            draggable: false,
            allowDrop: false,
            id: 'root'
        });
        treePanel.setRootNode(treeRoot);
        
        var initialTree = getInitialTree(translation);
        
        for (var i = 0; i < initialTree.length; i += 1) {
            var node = new Ext.tree.AsyncTreeNode(initialTree[i]);
            
            // check view right
            if (initialTree[i].viewRight && !Tine.Tinebase.common.hasRight('view', 'Admin', initialTree[i].viewRight)) {
                node.disabled = true;
            }
            
            treeRoot.appendChild(node);
        }
        
        treePanel.on('click', function (node, event) {
            
            if (node === null|| node.disabled) {
                return false;
            }
            
            var currentToolbar = Tine.Tinebase.MainScreen.getActiveToolbar();

            switch (node.attributes.dataPanelType) {
            case 'accesslog':
                Tine.Admin.accessLog.show();
                break;
            case 'accounts':
                Tine.Admin.user.show();
                break;
            case 'groups':
                Tine.Admin.Groups.Main.show();
                break;
            case 'computers':
                Tine.Admin.sambaMachine.show();
                break;
            case 'applications':
                Tine.Admin.Applications.Main.show();
                break;
            case 'sharedtags':
                Tine.Admin.Tags.Main.show();
                break;
            case 'roles':
                Tine.Admin.Roles.Main.show();
                break;
            case 'containers':
                Tine.Admin.container.show();
                break;
           case 'customfields':
                Tine.Admin.customfield.show();
                break;
           case 'serverinfo':
               Tine.Admin.getServerInfo(function(response) {
                   Tine.log.debug('Tine.Admin.getServerInfo()');
                   
                   if (! this.infoPanel) {
                       this.infoPanel = new Ext.Panel({
                           html: response.html,
                           autoScroll: true
                       });
                   } else {
                       this.infoPanel.update(response.html);
                   }
                   if (! this.infoPanelToolbar) {
                       // TODO add correct Tine 2.0 Toolbar layout with border
                       this.infoPanelToolbar = new Ext.Toolbar({items: [{
                           text: translation.gettext('Refresh'),
                           handler: function() {
                               node.fireEvent('click', node);
                           },
                           iconCls: 'action_login',
                           scale: 'medium',
                           rowspan: 2,
                           iconAlign: 'top'
                       }]});
                   }
                   Tine.Tinebase.MainScreen.setActiveContentPanel(this.infoPanel, true);
                   Tine.Tinebase.MainScreen.setActiveToolbar(this.infoPanelToolbar, true);
               }, this);
               break;
            }
        }, this);

        treePanel.on('beforeexpand', function (panel) {
            if (panel.getSelectionModel().getSelectedNode() === null) {
                panel.expandPath('/root');
                // don't open 'applications' if user has no right to manage apps
                if (Tine.Tinebase.common.hasRight('manage', 'Admin', 'accounts')) {
                    panel.selectPath('/root/accounts');
                } else {
                    treeRoot.eachChild(function (node) {
                        if (Tine.Tinebase.common.hasRight('manage', 'Admin', node.attributes.viewRight)) {
                            panel.selectPath('/root/' + node.id);
                            return;
                        }
                    }, this);
                }
            }
            panel.fireEvent('click', panel.getSelectionModel().getSelectedNode());
        }, this);

        treePanel.on('contextmenu', function (node, event) {
            event.stopEvent();
            //node.select();
            //node.getOwnerTree().fireEvent('click', _node);
            /* switch(node.attributes.contextMenuClass) {
                case 'ctxMenuContactsTree':
                    ctxMenuContactsTree.showAt(event.getXY());
                    break;
            } */
        });

        return treePanel;
    };
    
    // public functions and variables
    return {
        getPanel: getAdminTree
    };
    
}();
/*
 * Tine 2.0
 * 
 * @package     Admin
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schuele <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2010 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */

Ext.namespace('Tine.Admin');

/**
 * admin settings panel
 * 
 * @namespace   Tine.Admin
 * @class       Tine.Admin.AdminPanel
 * @extends     Tine.widgets.dialog.AdminPanel
 * 
 * <p>Admin Admin Panel</p>
 * <p><pre>
 * </pre></p>
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schuele <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2010 Metaways Infosystems GmbH (http://www.metaways.de)
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Admin.AdminPanel
 */
Tine.Admin.AdminPanel = Ext.extend(Tine.widgets.dialog.AdminPanel, {
    /**
     * @private
     */
    appName: 'Admin',
    
    /**
     * get config items
     * 
     * @return {Array}
     */
    getConfigItems: function() {
        return [[{
            xtype: 'tinerecordpickercombobox',
            fieldLabel: this.app.i18n._('Default Addressbook for new contacts and groups'),
            name: 'defaultInternalAddressbook',
            blurOnSelect: true,
            recordClass: Tine.Tinebase.Model.Container,
            recordProxy: Tine.Admin.sharedAddressbookBackend
        }]];
    }
});

/**
 * admin panel on update function
 * 
 * TODO         update registry without reloading the mainscreen
 */
Tine.Admin.AdminPanel.onUpdate = function() {
    // reload mainscreen to make sure registry gets updated
    window.location = window.location.href.replace(/#+.*/, '');
}

/**
 * Admin admin settings popup
 * 
 * @param   {Object} config
 * @return  {Ext.ux.Window}
 */
Tine.Admin.AdminPanel.openWindow = function (config) {
    var window = Tine.WindowFactory.getWindow({
        width: 600,
        height: 400,
        name: Tine.Admin.AdminPanel.prototype.windowNamePrefix + Ext.id(),
        contentPanelConstructor: 'Tine.Admin.AdminPanel',
        contentPanelConstructorConfig: config
    });
    return window;
};
/*
 * Tine 2.0
 * 
 * @package     Admin
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2009-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.ns('Tine.Admin.Model');

/**
 * @namespace   Tine.Admin.Model
 * @class       Tine.Admin.Model.TagRight
 * @extends     Ext.data.Record
 * 
 * TagRight Record Definition
 */ 
Tine.Admin.Model.TagRight = Ext.data.Record.create([
    {name: 'account_id'},
    {name: 'account_type'},
    {name: 'account_name'},
    {name: 'account_data'},
    {name: 'view_right', type: 'boolean'},
    {name: 'use_right',  type: 'boolean'}
]);

/**
 * @namespace   Tine.Admin.Model
 * @class       Tine.Admin.Model.AccessLog
 * @extends     Tine.Tinebase.data.Record
 * 
 * AccessLog Record Definition
 */ 
Tine.Admin.Model.AccessLog = Tine.Tinebase.data.Record.create([
    {name: 'sessionid'},
    {name: 'login_name'},
    {name: 'ip'},
    {name: 'li', type: 'date', dateFormat: Date.patterns.ISO8601Long},
    {name: 'lo', type: 'date', dateFormat: Date.patterns.ISO8601Long},
    {name: 'id'},
    {name: 'account_id'},
    {name: 'result'},
    {name: 'clienttype'}
], {
    appName: 'Admin',
    modelName: 'AccessLog',
    idProperty: 'id',
    titleProperty: 'login_name',
    // ngettext('Access Log', 'Access Logs', n);
    recordName: 'AccessLog',
    recordsName: 'AccessLogs'
});

/**
 * AccessLog data proxy
 * 
 * @type Tine.Tinebase.data.RecordProxy
 */ 
Tine.Admin.accessLogBackend = new Tine.Tinebase.data.RecordProxy({
    appName: 'Admin',
    modelName: 'AccessLog',
    recordClass: Tine.Admin.Model.AccessLog,
    idProperty: 'id'
});

/**
 * @namespace Tine.Admin.Model
 * @class     Tine.Admin.Model.Group
 * @extends   Tine.Admin.data.Record
 * 
 * Model of a user group
 */
Tine.Admin.Model.Group = Tine.Tinebase.data.Record.create([
    {name: 'id'},
    {name: 'name'},
    {name: 'description'},
    {name: 'container_id'},
    {name: 'visibility'}
    //{name: 'groupMembers'}
], {
    appName: 'Admin',
    modelName: 'Group',
    idProperty: 'id',
    titleProperty: 'name',
    // ngettext('Group', 'Groups', n); gettext('Groups');
    recordName: 'Group',
    recordsName: 'Groups',
    containerProperty: null
});

/**
 * returns default group data
 * 
 * @namespace Tine.Admin.Model.Group
 * @static
 * @return {Object} default data
 */
Tine.Admin.Model.Group.getDefaultData = function () {
    var internalAddressbook = Tine.Admin.registry.get('defaultInternalAddressbook');
    
    return {
        visibility: (internalAddressbook !== null) ? 'displayed' : 'hidden',
        container_id: internalAddressbook
    };
};

/**
 * @namespace Tine.Admin.Model
 * @class     Tine.Admin.Model.Application
 * @extends   Tine.Admin.data.Record
 * 
 * Model of an application
 */
Tine.Admin.Model.Application = Tine.Tinebase.data.Record.create([
    {name: 'id'},
    {name: 'name'},
    {name: 'status'},
    {name: 'order'},
    {name: 'app_tables'},
    {name: 'version'}
], {
    appName: 'Admin',
    modelName: 'Application',
    idProperty: 'id',
    titleProperty: 'name',
    // ngettext('Application', 'Applications', n); gettext('Applications');
    recordName: 'Application',
    recordsName: 'Applications',
    containerProperty: null
});
/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 * TODO         refactor this
 * TODO         translate strings (enable/disable/settings)
 */
 
Ext.ns('Tine.Admin', 'Tine.Admin.Applications');

/*********************************** MAIN DIALOG ********************************************/

Tine.Admin.Applications.Main = function() {

    // references to created toolbar and grid panel
    var applicationToolbar = null;
    var grid_applications = null;
    
    /**
     * onclick handler for edit action
     * 
     * TODO     make that more generic?
     */
    var _settingsHandler = function(_button, _event) {
        var selModel = Ext.getCmp('gridAdminApplications').getSelectionModel();
        if (selModel.getCount() > 0) {
            var selectedRows = selModel.getSelections();
            var appName = selectedRows[0].data.name;
            if (Tine[appName]) {
                _openSettingsWindow(appName);
            }
        } else {
            _button.setDisabled(true);
        }
    };
    
    var _openSettingsWindow = function(appName) {
        Tine[appName].AdminPanel.openWindow({
            record: (Tine[appName].Model.Settings) ? new Tine[appName].Model.Settings(appName) : null,
            title: String.format(_('{0} Settings'), translateAppTitle(appName)),
            listeners: {
                scope: this,
                'update': (Tine[appName].AdminPanel.onUpdate) ? Tine[appName].AdminPanel.onUpdate : Ext.emptyFn
            }
        });
    }

    var _enableDisableButtonHandler = function(state) {
        var applicationIds = new Array();
        var selectedRows = Ext.getCmp('gridAdminApplications').getSelectionModel().getSelections();
        for (var i = 0; i < selectedRows.length; ++i) {
            applicationIds.push(selectedRows[i].id);
        }
        
        Ext.Ajax.request({
            url : 'index.php',
            method : 'post',
            params : {
                method : 'Admin.setApplicationState',
                applicationIds : applicationIds,
                state: state
            },
            callback : function(_options, _success, _response) {
                if(_success === true) {
                    var result = Ext.util.JSON.decode(_response.responseText);
                    if(result.success === true) {
                        //Ext.getCmp('gridAdminApplications').getStore().reload();
                        // reload mainscreen because apps have to be loaded / unloaded
                        window.location = window.location.href.replace(/#+.*/, '');
                    }
                }
            }
        });
    };
    
    var translation = new Locale.Gettext();
    translation.textdomain('Admin');
    
    var _action_enable = new Ext.Action({
        text: translation.gettext('Enable Application'),
        disabled: true,
        handler: _enableDisableButtonHandler.createDelegate(this, ['enabled']),
        iconCls: 'action_enable'
    });

    var _action_disable = new Ext.Action({
        text: translation.gettext('Disable Application'),
        disabled: true,
        handler: _enableDisableButtonHandler.createDelegate(this, ['disabled']),
        iconCls: 'action_disable'
    });

    var _action_settings = new Ext.Action({
        text: translation.gettext('Settings'),
        disabled: true,
        handler: _settingsHandler,
        iconCls: 'action_settings'
    });

    var _createApplicationaDataStore = function()
    {
        /**
         * the datastore for lists
         */
        var ds_applications = new Ext.data.JsonStore({
            url: 'index.php',
            baseParams: {
                method: 'Admin.getApplications'
            },
            root: 'results',
            totalProperty: 'totalcount',
            id: 'id',
            fields: Tine.Admin.Model.Application,
            // turn on remote sorting
            remoteSort: true
        });
        
        ds_applications.setDefaultSort('name', 'asc');

        ds_applications.on('beforeload', function(_dataSource, _options) {
            _options = _options || {};
            _options.params = _options.params || {};
            _options.params.filter = Ext.getCmp('ApplicationsAdminQuickSearchField').getValue();
        });
        
        //ds_applications.load({params:{start:0, limit:50}});
        
        return ds_applications;
    };

    var _showApplicationsToolbar = function()
    {
        // if toolbar was allready created set active toolbar and return
        if (applicationToolbar)
        {
            Tine.Tinebase.MainScreen.setActiveToolbar(applicationToolbar, true);
            return;
        }
        
        this.translation = new Locale.Gettext();
        this.translation.textdomain('Admin');
        
        _action_enable.setText(this.translation.gettext('enable application'));
        _action_disable.setText(this.translation.gettext('disable application'));
        //_action_settings.setText(this.translation.gettext('settings'));
    
        var ApplicationsAdminQuickSearchField = new Ext.ux.SearchField({
            id: 'ApplicationsAdminQuickSearchField',
            width:240,
            emptyText: Tine.Tinebase.translation._hidden('enter searchfilter')
        });
        ApplicationsAdminQuickSearchField.on('change', function() {
            Ext.getCmp('gridAdminApplications').getStore().load({params:{start:0, limit:50}});
        });
        
        applicationToolbar = new Ext.Toolbar({
            id: 'toolbarAdminApplications',
            split: false,
            //height: 26,
            items: [{
                xtype: 'buttongroup',
                columns: 7,
                items: [
                    Ext.apply(new Ext.Button(_action_enable), {
                    scale: 'medium',
                    rowspan: 2,
                    iconAlign: 'top'
                    }), {xtype: 'tbspacer', width: 10},
                    Ext.apply(new Ext.Button(_action_disable), {
                        scale: 'medium',
                        rowspan: 2,
                        iconAlign: 'top'
                    }), {xtype: 'tbspacer', width: 10},
                    {xtype: 'tbseparator'}, {xtype: 'tbspacer', width: 10},
                    Ext.apply(new Ext.Button(_action_settings), {
                        scale: 'medium',
                        rowspan: 2,
                        iconAlign: 'top'
                    })
                ]
            }, '->',
                this.translation.gettext('Search:'), ' ',
//                new Ext.ux.SelectBox({
//                    listClass:'x-combo-list-small',
//                      width:90,
//                      value:'Starts with',
//                      id:'search-type',
//                      store: new Ext.data.SimpleStore({
//                        fields: ['text'],
//                        expandData: true,
//                        data : ['Starts with', 'Ends with', 'Any match']
//                      }),
//                      displayField: 'text'
//                }),
                ' ',
                ApplicationsAdminQuickSearchField
            ]
        });
        
        Tine.Tinebase.MainScreen.setActiveToolbar(applicationToolbar, true);
    };
    
    /**
     * translate and return app title
     * 
     * TODO try to generalize this fn as this gets used in Tags.js + RoleEditDialog.js as well 
     *      -> this could be moved to Tine.Admin.Application after Admin js refactoring
     */
    var translateAppTitle = function(appName) {
        var app = Tine.Tinebase.appMgr.get(appName);
        return (app) ? app.getTitle() : appName;
    };

    /**
     * render enabled field (translate)
     */
    var _renderEnabled = function(_value, _cellObject, _record, _rowIndex, _colIndex, _dataStore) {
        var translation = new Locale.Gettext();
        translation.textdomain('Admin');
        
        var gridValue;
        
        switch(_value) {
            case 'disabled':
                gridValue = translation.gettext('disabled');
                break;
            case 'enabled':
              gridValue = translation.gettext('enabled');
              break;
              
            default:
              gridValue = String.format(translation.gettext('unknown status ({0})'), value);
              break;
        }
        
        return gridValue;
    };

    /**
     * creates the address grid
     * 
     */
    var _showApplicationsGrid = function() 
    {
        // if grid panel was allready created set active content panel and return
        if (grid_applications) {
            Tine.Tinebase.MainScreen.setActiveContentPanel(grid_applications, true);
            return;
        }
        
        var ctxMenuGrid = new Ext.menu.Menu({
            items: [
                _action_enable,
                _action_disable,
                _action_settings
            ]
        });

        
        var ds_applications = _createApplicationaDataStore();
        
        var pagingToolbar = new Ext.PagingToolbar({ // inline paging toolbar
            pageSize: 50,
            store: ds_applications,
            displayInfo: true,
            displayMsg: this.translation.gettext('Displaying application {0} - {1} of {2}'),
            emptyMsg: this.translation.gettext("No applications to display")
        });
        
        var cm_applications = new Ext.grid.ColumnModel({
            defaults: {
                sortable: true,
                resizable: true
            },
            columns: [
                { header: this.translation.gettext('Order'),   id: 'order', dataIndex: 'order', width: 50},
                { header: this.translation.gettext('Name'),    id: 'name', dataIndex: 'name', renderer: translateAppTitle},
                { header: this.translation.gettext('Status'),  id: 'status', dataIndex: 'status', width: 150, renderer: _renderEnabled},
                { header: this.translation.gettext('Version'), id: 'version', dataIndex: 'version', width: 70}
            ]
        });

        var rowSelectionModel = new Ext.grid.RowSelectionModel({multiSelect:true});
        
        rowSelectionModel.on('selectionchange', function(_selectionModel) {
            var rowCount = _selectionModel.getCount();
            var selected = _selectionModel.getSelections();

            if ( Tine.Tinebase.common.hasRight('manage', 'Admin', 'apps') ) {
                if (rowCount < 1) {
                    _action_enable.setDisabled(true);
                    _action_disable.setDisabled(true);
                    _action_settings.setDisabled(true);
                } else if (rowCount > 1) {
                    _action_enable.setDisabled(false);
                    _action_disable.setDisabled(false);
                    _action_settings.setDisabled(true);
                } else {
                    _action_enable.setDisabled(false);
                    _action_disable.setDisabled(false);
                    // check if app has admin panel and is enabled
                    if (Tine[selected[0].data.name] && Tine[selected[0].data.name].AdminPanel && selected[0].data.status == 'enabled') {
                        _action_settings.setDisabled(false);
                    } else {
                        _action_settings.setDisabled(true);
                    }
                }
                
                // don't allow to disable Admin, Tinebase or Addressbook as we can't deal with this yet
                for (var i=0; i<selected.length; i++) {
                    if (typeof selected[i].get == 'function' && selected[i].get('name').toString().match(/Tinebase|Admin|Addressbook/)) {
                        _action_enable.setDisabled(true);
                        _action_disable.setDisabled(true);
                        break;
                    }
                }
            }
        });
                
        grid_applications = new Ext.grid.GridPanel({
            id: 'gridAdminApplications',
            store: ds_applications,
            cm: cm_applications,
            tbar: pagingToolbar,     
            autoSizeColumns: false,
            selModel: rowSelectionModel,
            enableColLock:false,
            autoExpandColumn: 'name',
            border: false,
            viewConfig: {
                /**
                 * Return CSS class to apply to rows depending upon flags
                 * - checks Flagged, Deleted and Seen
                 * 
                 * @param {} record
                 * @param {} index
                 * @return {String}
                 */
                getRowClass: function(record, index) {
                    //console.log(record);
                    var className = '';
                    switch(record.get('status')) {
                        case 'disabled':
                            className = 'grid_row_disabled';
                            break;
                        case 'enabled':
                            className = 'grid_row_enabled';
                            break;
                    }
                    return className;
                }
            }
        });
        
        Tine.Tinebase.MainScreen.setActiveContentPanel(grid_applications, true);
        
        grid_applications.on('rowcontextmenu', function(_grid, _rowIndex, _eventObject) {
            _eventObject.stopEvent();
            if(!_grid.getSelectionModel().isSelected(_rowIndex)) {
                _grid.getSelectionModel().selectRow(_rowIndex);

                if ( Tine.Tinebase.common.hasRight('manage', 'Admin', 'apps') ) {
                    _action_enable.setDisabled(false);
                    _action_disable.setDisabled(false);
                }
                
                // don't allow to disable Admin, Tinebase or Addressbook as we can't deal with this yet
                if(_grid.getSelectionModel().getSelected().get('name').toString().match(/Tinebase|Admin|Addressbook/)) {
                    _action_enable.setDisabled(true);
                    _action_disable.setDisabled(true);
                }
            }
            ctxMenuGrid.showAt(_eventObject.getXY());
        }, this);
        
        grid_applications.on('rowdblclick', function(grid, index, e) {
            var record = grid.getStore().getAt(index);
            if (Tine[record.data.name].AdminPanel && record.data.status == 'enabled') {
                _openSettingsWindow(record.data.name);
            }
        }, this);
          
        return;
    };
    
    // public functions and variables
    return {
        show: function() 
        {
            _showApplicationsToolbar();
            _showApplicationsGrid();
            
            this.loadData();
        },
        
        loadData: function()
        {
            var dataStore = Ext.getCmp('gridAdminApplications').getStore();
            dataStore.load({ params: { start:0, limit:50 } });
        },
        
        reload: function() 
        {
            if(Ext.ComponentMgr.all.containsKey('gridAdminApplications')) {
                setTimeout ("Ext.getCmp('gridAdminApplications').getStore().reload()", 200);
            }
        }
    };
    
}();
/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2007-2010 Metaways Infosystems GmbH (http://www.metaways.de)
 */

/*global Ext, Tine*/ 

Ext.ns('Tine.Admin.user');

/**
 * Users 'mainScreen'
 * 
 * @static
 */
Tine.Admin.user.show = function () {
    var app = Tine.Tinebase.appMgr.get('Admin');
    if (! Tine.Admin.user.gridPanel) {
        Tine.Admin.user.gridPanel = new Tine.Admin.user.GridPanel({
            app: app
        });
    }
    else {
        Tine.Admin.user.gridPanel.loadGridData.defer(100, Tine.Admin.user.gridPanel, []);
    }
    
    Tine.Tinebase.MainScreen.setActiveContentPanel(Tine.Admin.user.gridPanel, true);
    Tine.Tinebase.MainScreen.setActiveToolbar(Tine.Admin.user.gridPanel.actionToolbar, true);
};


/************** models *****************/
Ext.ns('Tine.Admin.Model');

/**
 * Model of an account
 */
Tine.Admin.Model.UserArray = [
    { name: 'accountId' },
    { name: 'accountFirstName' },
    { name: 'accountLastName' },
    { name: 'accountLoginName' },
    { name: 'accountPassword' },
    { name: 'accountDisplayName' },
    { name: 'accountFullName' },
    { name: 'accountStatus' },
    { name: 'groups' },
    { name: 'accountRoles' },
    { name: 'accountPrimaryGroup' },
    { name: 'accountExpires', type: 'date', dateFormat: Date.patterns.ISO8601Long },
    { name: 'accountLastLogin', type: 'date', dateFormat: Date.patterns.ISO8601Long },
    { name: 'accountLastPasswordChange', type: 'date', dateFormat: Date.patterns.ISO8601Long },
    { name: 'accountLastLoginfrom' },
    { name: 'accountEmailAddress' },
    { name: 'accountHomeDirectory' },
    { name: 'accountLoginShell' },
    { name: 'openid'},
    { name: 'visibility'},
    { name: 'sambaSAM' },
    { name: 'emailUser' },
    { name: 'contact_id' },
    { name: 'container_id' }
];

Tine.Admin.Model.User = Tine.Tinebase.data.Record.create(Tine.Admin.Model.UserArray, {
    appName: 'Admin',
    modelName: 'User',
    idProperty: 'accountId',
    titleProperty: 'accountDisplayName',
    // ngettext('User', 'Users', n); gettext('Users');
    recordName: 'User',
    recordsName: 'Users'
});

/**
 * returns default account data
 * 
 * @namespace Tine.Admin.Model.User
 * @static
 * @return {Object} default data
 */
Tine.Admin.Model.User.getDefaultData = function () {
    var internalAddressbook = Tine.Admin.registry.get('defaultInternalAddressbook'),
        emailUserDefaults = (Tine.Admin.registry.get('config').defaultImapUserSettings && Tine.Admin.registry.get('config').defaultImapUserSettings.value)
            ? Tine.Admin.registry.get('config').defaultImapUserSettings.value : '';
        
    return {
        sambaSAM: '',
        emailUser: emailUserDefaults,
        accountStatus: 'enabled',
        visibility: (internalAddressbook !== null) ? 'displayed' : 'hidden',
        container_id: internalAddressbook,
        accountPrimaryGroup: Tine.Admin.registry.get('defaultPrimaryGroup')
    };
};

Tine.Admin.Model.SAMUserArray = [
    { name: 'sid'              },
    { name: 'primaryGroupSID'  },
    { name: 'acctFlags'        },
    { name: 'homeDrive'        },
    { name: 'homePath'         },
    { name: 'profilePath'      },
    { name: 'logonScript'      },
    { name: 'logonTime',     type: 'date', dateFormat: Date.patterns.ISO8601Long },
    { name: 'logoffTime',    type: 'date', dateFormat: Date.patterns.ISO8601Long },
    { name: 'kickoffTime',   type: 'date', dateFormat: Date.patterns.ISO8601Long },
    { name: 'pwdLastSet',    type: 'date', dateFormat: Date.patterns.ISO8601Long },
    { name: 'pwdCanChange',  type: 'date', dateFormat: Date.patterns.ISO8601Long },
    { name: 'pwdMustChange', type: 'date', dateFormat: Date.patterns.ISO8601Long }
];

Tine.Admin.Model.SAMUser = Tine.Tinebase.data.Record.create(Tine.Admin.Model.SAMUserArray, {
    appName: 'Admin',
    modelName: 'SAMUser',
    idProperty: 'sid',
    titleProperty: null,
    // ngettext('Samba User', 'Samba Users', n);
    recordName: 'Samba User',
    recordsName: 'Samba Users'
});

Tine.Admin.Model.EmailUserArray = [
    { name: 'emailUID' },
    { name: 'emailGID' },
    { name: 'emailMailQuota' },
    { name: 'emailMailSize' },
    { name: 'emailSieveQuota' },
    { name: 'emailSieveSize' },
    { name: 'emailLastLogin', type: 'date', dateFormat: Date.patterns.ISO8601Long },
    { name: 'emailUserId' },
    { name: 'emailAliases' },
    { name: 'emailForwards' },
    { name: 'emailForwardOnly' },
    { name: 'emailAddress' },
    { name: 'emailUsername' }
];

Tine.Admin.Model.EmailUser = Tine.Tinebase.data.Record.create(Tine.Admin.Model.EmailUserArray, {
    appName: 'Admin',
    modelName: 'EmailUser',
    idProperty: 'sid',
    titleProperty: null,
    // ngettext('Email User', 'Email Users', n);
    recordName: 'Email User',
    recordsName: 'Email Users'
});




/************** backends *****************/

Tine.Admin.userBackend = new Tine.Tinebase.data.RecordProxy({
    appName: 'Admin',
    modelName: 'User',
    recordClass: Tine.Admin.Model.User,
    idProperty: 'accountId'
});

Tine.Admin.samUserBackend = new Tine.Tinebase.data.RecordProxy({
    appName: 'Admin',
    modelName: 'SAMUser',
    recordClass: Tine.Admin.Model.SAMUser,
    idProperty: 'sid'
});

Tine.Admin.emailUserBackend = new Tine.Tinebase.data.RecordProxy({
    appName: 'Admin',
    modelName: 'EmailUser',
    recordClass: Tine.Admin.Model.EmailUser,
    idProperty: 'emailUID'
});

Tine.Admin.sharedAddressbookBackend = new Tine.Tinebase.data.RecordProxy({
    appName: 'Admin',
    modelName: 'SharedAddressbook',
    recordClass: Tine.Tinebase.Model.Container,
    idProperty: 'id'
});
/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
Ext.ns('Tine.Admin.user');


/**
 * User grid panel
 * 
 * @namespace   Tine.Admin.user
 * @class       Tine.Admin.user.GridPanel
 * @extends     Tine.widgets.grid.GridPanel
 */
Tine.Admin.user.GridPanel = Ext.extend(Tine.widgets.grid.GridPanel, {
    /**
     * @property isLdapBackend
     * @type Boolean
     */
    isLdapBackend: false,
    
    newRecordIcon: 'action_addContact',
    recordClass: Tine.Admin.Model.User,
    recordProxy: Tine.Admin.userBackend,
    defaultSortInfo: {field: 'accountLoginName', direction: 'ASC'},
    evalGrants: false,
    gridConfig: {
        id: 'gridAdminUsers',
        autoExpandColumn: 'accountDisplayName'
    },
    
    initComponent: function() {
        this.gridConfig.cm = this.getColumnModel();
        this.initFilterToolbar();
        
        this.plugins = this.plugins || [];
        this.plugins.push(this.filterToolbar);
        
        this.isLdapBackend = Tine.Tinebase.registry.get('accountBackend') == 'Ldap';
            
        Tine.Admin.user.GridPanel.superclass.initComponent.call(this);
    },
    
    /**
     * init actions with actionToolbar, contextMenu and actionUpdater
     * 
     * @private
     */
    initActions: function() {
        this.actionEnable = new Ext.Action({
            requiredGrant: 'readGrant',
            text: this.app.i18n._('Enable Account'),
            allowMultiple: true,
            disabled: true,
            handler: this.enableDisableButtonHandler.createDelegate(this, ['enabled']),
            iconCls: 'action_enable',
            actionUpdater: this.enableDisableActionUpdater.createDelegate(this, [['disabled', 'blocked', 'expired']], true)
        });
    
        this.actionDisable = new Ext.Action({
            requiredGrant: 'readGrant',
            text: this.app.i18n._('Disable Account'),
            allowMultiple: true,
            disabled: true,
            handler: this.enableDisableButtonHandler.createDelegate(this, ['disabled']),
            iconCls: 'action_disable',
            actionUpdater: this.enableDisableActionUpdater.createDelegate(this, [['enabled']], true)
        });
    
        this.actionResetPassword = new Ext.Action({
            requiredGrant: 'readGrant',
            text: this.app.i18n._('Reset Password'),
            disabled: true,
            handler: this.resetPasswordHandler,
            iconCls: 'action_password',
            scope: this
        });
        
        this.actionUpdater.addActions([
            this.actionEnable,
            this.actionDisable,
            this.actionResetPassword
        ]);
        
        this.supr().initActions.call(this);
    },
    
    /**
     * initialises filter toolbar
     */
    initFilterToolbar: function() {
        this.filterToolbar = new Tine.widgets.grid.FilterToolbar({
            filterModels: [
                {label: this.app.i18n.n_('User', 'Users', 1),    field: 'query',       operators: ['contains']}
                //{label: this.app.i18n._('Description'),    field: 'description', operators: ['contains']},
            ],
            defaultFilter: 'query',
            filters: [],
            plugins: [
                new Tine.widgets.grid.FilterToolbarQuickFilterPlugin()
            ]
        });
    },    
    
    /**
     * add custom items to action toolbar
     * 
     * @return {Object}
     */
    getActionToolbarItems: function() {
        return [
            Ext.apply(new Ext.Button(this.actionEnable), {
                scale: 'medium',
                rowspan: 2,
                iconAlign: 'top'
            }),
            Ext.apply(new Ext.Button(this.actionDisable), {
                scale: 'medium',
                rowspan: 2,
                iconAlign: 'top'
            }),
            Ext.apply(new Ext.Button(this.actionResetPassword), {
                scale: 'medium',
                rowspan: 2,
                iconAlign: 'top'
            })
        ];
    },
    
    /**
     * add custom items to context menu
     * 
     * @return {Array}
     */
    getContextMenuItems: function() {
        var items = [
            '-',
            this.actionEnable,
            this.actionDisable,
            '-',
            this.actionResetPassword
        ];
        
        return items;
    },
    
    /**
     * returns column model
     * 
     * @return Ext.grid.ColumnModel
     * @private
     */
    getColumnModel: function() {
        return new Ext.grid.ColumnModel({
            defaults: {
                sortable: true,
                hidden: true,
                resizable: true
            },
            columns: this.getColumns()
        });
    },
    
    /**
     * returns columns
     * @private
     * @return Array
     */
    getColumns: function(){
        return [
            { header: this.app.i18n._('ID'), id: 'accountId', dataIndex: 'accountId', width: 50},
            { header: this.app.i18n._('Status'), id: 'accountStatus', dataIndex: 'accountStatus', hidden: this.isLdapBackend, width: 50, renderer: this.statusRenderer},
            { header: this.app.i18n._('Display name'), id: 'accountDisplayName', dataIndex: 'accountDisplayName', hidden: false},
            { header: this.app.i18n._('Login name'), id: 'accountLoginName', dataIndex: 'accountLoginName', width: 160, hidden: false},
            { header: this.app.i18n._('Last name'), id: 'accountLastName', dataIndex: 'accountLastName'},
            { header: this.app.i18n._('First name'), id: 'accountFirstName', dataIndex: 'accountFirstName'},
            { header: this.app.i18n._('Email'), id: 'accountEmailAddress', dataIndex: 'accountEmailAddress', width: 200, hidden: false},
            { header: this.app.i18n._('OpenID'), id: 'openid', dataIndex: 'openid', width: 200},
            { header: this.app.i18n._('Last login at'), id: 'accountLastLogin', dataIndex: 'accountLastLogin', hidden: this.isLdapBackend, width: 140, renderer: Tine.Tinebase.common.dateTimeRenderer},
            { header: this.app.i18n._('Last login from'), id: 'accountLastLoginfrom', hidden: this.isLdapBackend, dataIndex: 'accountLastLoginfrom'},
            { header: this.app.i18n._('Password changed'), id: 'accountLastPasswordChange', dataIndex: 'accountLastPasswordChange', width: 140, renderer: Tine.Tinebase.common.dateTimeRenderer, hidden: false},
            { header: this.app.i18n._('Expires'), id: 'accountExpires', dataIndex: 'accountExpires', width: 140, renderer: Tine.Tinebase.common.dateTimeRenderer, hidden: false}
        ];
    },
    
    enableDisableButtonHandler: function(status) {
        var accountIds = new Array();
        var selectedRows = this.grid.getSelectionModel().getSelections();
        for (var i = 0; i < selectedRows.length; ++i) {
            accountIds.push(selectedRows[i].id);
        }
        
        Ext.Ajax.request({
            url : 'index.php',
            method : 'post',
            params : {
                method : 'Admin.setAccountState',
                accountIds : accountIds,
                status: status
            },
            scope: this,
            callback : function(_options, _success, _response) {
                if(_success === true) {
                    var result = Ext.util.JSON.decode(_response.responseText);
                    if(result.success === true) {
                        this.loadGridData({
                            removeStrategy: 'keepBuffered'
                        });
                    }
                }
            }
        });
    },
    
    /**
     * updates enable/disable actions
     * 
     * @param {Ext.Action} action
     * @param {Object} grants grants sum of grants
     * @param {Object} records
     * @param {Array} requiredAccountStatus
     */
    enableDisableActionUpdater: function(action, grants, records, requiredAccountStatus) {
        var enabled = records.length > 0;
        Ext.each(records, function(record){
            enabled &= requiredAccountStatus.indexOf(record.get('accountStatus')) >=0;// === requiredAccountStatus;
            return enabled;
        }, this);
        
        action.setDisabled(!enabled);
    },
    
    /**
     * reset password
     * 
     * TODO add checkbox for must change pw
     * TODO add pw repeat (see user edit dialog)
     */
    resetPasswordHandler: function(_button, _event) {
        Ext.MessageBox.prompt(this.app.i18n._('Set new password'), this.app.i18n._('Please enter the new password:'), function(_button, _text) {
            if(_button == 'ok') {
                var accountObject = this.grid.getSelectionModel().getSelected().data;
                
                Ext.Ajax.request( {
                    params: {
                        method    : 'Admin.resetPassword',
                        account   : accountObject,
                        password  : _text,
                        mustChange: false
                    },
                    scope: this,
                    callback : function(_options, _success, _response) {
                        if(_success === true) {
                            var result = Ext.util.JSON.decode(_response.responseText);
                            if(result.success === true) {
                                this.grid.getStore().reload();
                            }
                        } else {
                            Tine.Tinebase.ExceptionHandler.handleRequestException(_response);
                        }
                    }
                });
            }
        }, this);
    },
    
    statusRenderer: function (_value, _cellObject, _record, _rowIndex, _colIndex, _dataStore) {
        var gridValue;
        
        switch(_value) {
            case 'blocked':
                gridValue = "<img src='images/oxygen/16x16/status/security-medium.png' width='12' height='12'/>";
                break;

            case 'enabled':
                gridValue = "<img src='images/oxygen/16x16/actions/dialog-apply.png' width='12' height='12'/>";
                break;
              
            case 'disabled':
                gridValue = "<img src='images/oxygen/16x16/actions/dialog-cancel.png' width='12' height='12'/>";
                break;
              
            case 'expired':
                gridValue = "<img src='images/oxygen/16x16/status/user-away.png' width='12' height='12'/>";
                break;

            default:
                gridValue = _value;
                break;
        }
        
        return gridValue;
    }
});
/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2009-2012 Metaways Infosystems GmbH (http://www.metaways.de)
 */
 
/*global Ext, Tine*/

Ext.ns('Tine.Admin.user');

/**
 * @namespace   Tine.Admin.user
 * @class       Tine.Admin.UserEditDialog
 * @extends     Tine.widgets.dialog.EditDialog
 * 
 * NOTE: this class dosn't use the user namespace as this is not yet supported by generic grid
 * 
 * <p>User Edit Dialog</p>
 * <p>
 * </p>
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Admin.UserEditDialog
 */
Tine.Admin.UserEditDialog = Ext.extend(Tine.widgets.dialog.EditDialog, {
    
    /**
     * @private
     */
    windowNamePrefix: 'userEditWindow_',
    appName: 'Admin',
    recordClass: Tine.Admin.Model.User,
    recordProxy: Tine.Admin.userBackend,
    evalGrants: false,
    
    /**
     * @private
     */
    initComponent: function () {
        var accountBackend = Tine.Tinebase.registry.get('accountBackend');
        this.ldapBackend = (accountBackend === 'Ldap');

        Tine.Admin.UserEditDialog.superclass.initComponent.call(this);
    },

    /**
     * @private
     */
    onRecordLoad: function () {
        // interrupt process flow until dialog is rendered
        if (! this.rendered) {
            this.onRecordLoad.defer(250, this);
            return;
        }
        
        // samba user
        var response = {
            responseText: Ext.util.JSON.encode(this.record.get('sambaSAM'))
        };
        this.samRecord = Tine.Admin.samUserBackend.recordReader(response);
        // email user
        var emailResponse = {
            responseText: Ext.util.JSON.encode(this.record.get('emailUser'))
        };
        this.emailRecord = Tine.Admin.emailUserBackend.recordReader(emailResponse);
        
        // format dates
        var dateTimeDisplayFields = ['accountLastLogin', 'accountLastPasswordChange', 'logonTime', 'logoffTime', 'pwdLastSet'];
        for (var i = 0; i < dateTimeDisplayFields.length; i += 1) {
            if (dateTimeDisplayFields[i] === 'accountLastLogin' || dateTimeDisplayFields[i] === 'accountLastPasswordChange') {
                this.record.set(dateTimeDisplayFields[i], Tine.Tinebase.common.dateTimeRenderer(this.record.get(dateTimeDisplayFields[i])));
            } else {
                this.samRecord.set(dateTimeDisplayFields[i], Tine.Tinebase.common.dateTimeRenderer(this.samRecord.get(dateTimeDisplayFields[i])));
            }
        }

        this.getForm().loadRecord(this.emailRecord);
        this.getForm().loadRecord(this.samRecord);
        this.record.set('sambaSAM', this.samRecord.data);

        if (Tine.Admin.registry.get('manageSmtpEmailUser')) {
            if (this.emailRecord.get('emailAliases')) {
                this.aliasesGrid.setStoreFromArray(this.emailRecord.get('emailAliases'));
            }
            if (this.emailRecord.get('emailForwards')) {
                this.forwardsGrid.setStoreFromArray(this.emailRecord.get('emailForwards'));
            }
        }
        
        // load stores for memberships
        if (this.record.id) {
            this.storeGroups.loadData(this.record.get('groups'));
            this.storeRoles.loadData(this.record.get('accountRoles'));
        }
        
        Tine.Admin.UserEditDialog.superclass.onRecordLoad.call(this);
    },
    
    /**
     * @private
     */
    onRecordUpdate: function () {
        Tine.Admin.UserEditDialog.superclass.onRecordUpdate.call(this);
        
        Tine.log.debug('Tine.Admin.UserEditDialog::onRecordUpdate()');
        
        var form = this.getForm();
        form.updateRecord(this.samRecord);
        if (this.samRecord.dirty) {
            // only update sam record if something changed
            this.unsetLocalizedDateTimeFields(this.samRecord, ['logonTime', 'logoffTime', 'pwdLastSet']);
            this.record.set('sambaSAM', '');
            this.record.set('sambaSAM', this.samRecord.data);
        }

        form.updateRecord(this.emailRecord);
        // get aliases / forwards
        if (Tine.Admin.registry.get('manageSmtpEmailUser')) {
            // forcing blur of quickadd grids
            this.aliasesGrid.doBlur();
            this.forwardsGrid.doBlur();
            this.emailRecord.set('emailAliases', this.aliasesGrid.getFromStoreAsArray());
            this.emailRecord.set('emailForwards', this.forwardsGrid.getFromStoreAsArray());
            Tine.log.debug('Tine.Admin.UserEditDialog::onRecordUpdate() -> setting aliases and forwards in email record');
            Tine.log.debug(this.emailRecord);
        }
        this.unsetLocalizedDateTimeFields(this.emailRecord, ['emailLastLogin']);
        this.record.set('emailUser', '');
        this.record.set('emailUser', this.emailRecord.data);
        
        var newGroups = [],
            newRoles = [];
        
        this.storeGroups.each(function (rec) {
            newGroups.push(rec.data.id);
        });
        // add selected primary group to new groups if not exists
        if (newGroups.indexOf(this.record.get('accountPrimaryGroup')) === -1) {
            newGroups.push(this.record.get('accountPrimaryGroup'));
        }
         
        this.storeRoles.each(function (rec) {
            newRoles.push(rec.data.id);
        });
        
        this.record.set('groups', newGroups);
        this.record.set('accountRoles', newRoles);
        
        this.unsetLocalizedDateTimeFields(this.record, ['accountLastLogin', 'accountLastPasswordChange']);
    },
    
    /**
     * need to unset localized datetime fields before saving
     * 
     * @param {Object} record
     * @param {Array} dateTimeDisplayFields
     */
    unsetLocalizedDateTimeFields: function(record, dateTimeDisplayFields) {
        Ext.each(dateTimeDisplayFields, function (dateTimeDisplayField) {
            record.set(dateTimeDisplayField, '');
        }, this);
    },

    /**
     * is form valid?
     * 
     * @return {Boolean}
     */
    isValid: function() {
        var result = Tine.Admin.UserEditDialog.superclass.isValid.call(this);
        if (! result) {
            return false;
        }
        
        if (Tine.Admin.registry.get('manageSmtpEmailUser')) {
            var emailValue = this.getForm().findField('accountEmailAddress').getValue();
            if (! this.checkEmailDomain(emailValue)) {
                result = false;
                this.getForm().markInvalid([{
                    id: 'accountEmailAddress',
                    msg: this.app.i18n._("Domain is not allowed. Check your SMTP domain configuration.")
                }]);
            }
        }
        
        return result;
    },
    
    /**
     * check valid email domain (if email domain is set in config)
     * 
     * @param {String} email
     * @return {Boolean}
     * 
     * TODO use this for smtp aliases, too
     */
    checkEmailDomain: function(email) {
        if (! Tine.Admin.registry.get('primarydomain') || ! email) {
            return true;
        }
        
        var allowedDomains = [Tine.Admin.registry.get('primarydomain')],
            emailDomain = email.split('@')[1];
            
        if (Ext.isString(Tine.Admin.registry.get('secondarydomains'))) {
            allowedDomains = allowedDomains.concat(Tine.Admin.registry.get('secondarydomains').split(','));
        }
        
        return (allowedDomains.indexOf(emailDomain) !== -1);
    },
    
    /**
     * Validate confirmed password
     */
    onPasswordConfirm: function () {
        var confirmForm = this.passwordConfirmWindow.items.first().getForm(),
            confirmValues = confirmForm.getValues(),
            passwordStatus = confirmForm.findField('passwordStatus'),
            passwordField = (this.getForm()) ? this.getForm().findField('accountPassword') : null;
        
        if (! passwordField) {
            // oops: something went wrong, this should not happen
            return false;
        }
        
        if (confirmValues.passwordRepeat !== passwordField.getValue()) {
            passwordStatus.el.setStyle('color', 'red');
            passwordStatus.setValue(this.app.i18n.gettext('Passwords do not match!'));
            
            passwordField.passwordsMatch = false;
            passwordField.markInvalid(this.app.i18n.gettext('Passwords do not match!'));
        } else {
            passwordStatus.el.setStyle('color', 'green');
            passwordStatus.setValue(this.app.i18n.gettext('Passwords match!'));
                        
            passwordField.passwordsMatch = true;
            passwordField.clearInvalid();
        }
        
        return passwordField.passwordsMatch ? passwordField.passwordsMatch : passwordStatus.getValue();
    },
    
    /**
     * Get current primary group (selected from combobox or default primary group)
     * 
     * @return {String} - id of current primary group
     */
    getCurrentPrimaryGroupId: function () {
        return this.getForm().findField('accountPrimaryGroup').getValue() || this.record.get('accountPrimaryGroup').id;
    },
    
    /**
     * Init User groups picker grid
     * 
     * @return {Tine.widgets.account.PickerGridPanel}
     */
    initUserGroups: function () {
        this.storeGroups = new Ext.data.JsonStore({
            root: 'results',
            totalProperty: 'totalcount',
            id: 'id',
            fields: Tine.Admin.Model.Group
        });
        
        var self = this;
        
        this.pickerGridGroups = new Tine.widgets.account.PickerGridPanel({
            border: false,
            frame: false,
            store: this.storeGroups,
            selectType: 'group',
            selectAnyone: false,
            selectTypeDefault: 'group',
            groupRecordClass: Tine.Admin.Model.Group,
            getColumnModel: function () {
                return new Ext.grid.ColumnModel({
                    defaults: { sortable: true },
                    columns:  [
                        {id: 'name', header: _('Name'), dataIndex: this.recordPrefix + 'name', renderer: function (val, meta, record) {
                            return record.data.id === self.getCurrentPrimaryGroupId() ? (record.data.name + '<span class="x-item-disabled"> (' + self.app.i18n.gettext('Primary group') + ')<span>') : record.data.name;
                        }}
                    ]
                });
            }
        });
        // disable remove of group if equal to current primary group
        this.pickerGridGroups.selModel.on('beforerowselect', function (sm, index, keep, record) {
            if (record.data.id === this.getCurrentPrimaryGroupId()) {
                return false;
            }
        }, this);
        
        return this.pickerGridGroups;
    },
    
    /**
     * Init User roles picker grid
     * 
     * @return {Tine.widgets.account.PickerGridPanel}
     */
    initUserRoles: function () {
        this.storeRoles = new Ext.data.JsonStore({
            root: 'results',
            totalProperty: 'totalcount',
            id: 'id',
            fields: Tine.Tinebase.Model.Role
        });
        
        this.pickerGridRoles = new Tine.widgets.grid.PickerGridPanel({
            border: false,
            frame: false,
            autoExpandColumn: 'name',
            store: this.storeRoles,
            recordClass: Tine.Tinebase.Model.Role,
            columns: [{id: 'name', header: Tine.Tinebase.translation.gettext('Name'), sortable: true, dataIndex: 'name'}],
            initActionsAndToolbars: function () {
                // for now removed abillity to edit role membership
//                Tine.widgets.grid.PickerGridPanel.prototype.initActionsAndToolbars.call(this);
//                
//                this.comboPanel = new Ext.Container({
//                    layout: 'hfit',
//                    border: false,
//                    items: this.getSearchCombo(),
//                    columnWidth: 1
//                });
//                
//                this.tbar = new Ext.Toolbar({
//                    items: this.comboPanel,
//                    layout: 'column'
//                });
            },
            onAddRecordFromCombo: function (recordToAdd) {
                // check if already in
                if (! this.recordStore.getById(recordToAdd.id)) {
                    this.recordStore.add([recordToAdd]);
                }
                this.collapse();
                this.clearValue();
                this.reset();
            }
        });
        // remove listeners for this grid selection model
        this.pickerGridRoles.selModel.purgeListeners();
        
        return this.pickerGridRoles;
    },
    
    /**
     * Init Fileserver tab items
     * 
     * @return {Array} - array ff fileserver tab items
     */
    initFileserver: function () {
        if (this.ldapBackend) {
            return [{
                xtype: 'fieldset',
                title: this.app.i18n.gettext('Unix'),
                autoHeight: true,
                checkboxToggle: false,
                layout: 'hfit',
                items: [{
                    xtype: 'columnform',
                    labelAlign: 'top',
                    formDefaults: {
                        xtype: 'textfield',
                        anchor: '100%',
                        labelSeparator: '',
                        columnWidth: 0.333
                    },
                    items: [[{
                        fieldLabel: this.app.i18n.gettext('Home Directory'),
                        name: 'accountHomeDirectory',
                        columnWidth: 0.666
                    }, {
                        fieldLabel: this.app.i18n.gettext('Login Shell'),
                        name: 'accountLoginShell'
                    }]]
                }]
            }, {
                xtype: 'fieldset',
                title: this.app.i18n.gettext('Windows'),
                autoHeight: true,
                checkboxToggle: false,
                layout: 'hfit',
                items: [{
                    xtype: 'columnform',
                    labelAlign: 'top',
                    formDefaults: {
                        xtype: 'textfield',
                        anchor: '100%',
                        labelSeparator: '',
                        columnWidth: 0.333
                    },
                    items: [[{
                        fieldLabel: this.app.i18n.gettext('Home Drive'),
                        name: 'homeDrive',
                        columnWidth: 0.666
                    }, {
                        xtype: 'displayfield',
                        fieldLabel: this.app.i18n.gettext('Logon Time'),
                        name: 'logonTime',
                        emptyText: this.app.i18n.gettext('never logged in'),
                        style: this.displayFieldStyle
                    }], [{
                        fieldLabel: this.app.i18n.gettext('Home Path'),
                        name: 'homePath',
                        columnWidth: 0.666
                    }, {
                        xtype: 'displayfield',
                        fieldLabel: this.app.i18n.gettext('Logoff Time'),
                        name: 'logoffTime',
                        emptyText: this.app.i18n.gettext('never logged off'),
                        style: this.displayFieldStyle
                    }], [{
                        fieldLabel: this.app.i18n.gettext('Profile Path'),
                        name: 'profilePath',
                        columnWidth: 0.666
                    }, {
                        xtype: 'displayfield',
                        fieldLabel: this.app.i18n.gettext('Password Last Set'),
                        name: 'pwdLastSet',
                        emptyText: this.app.i18n.gettext('never'),
                        style: this.displayFieldStyle
                    }], [{
                        fieldLabel: this.app.i18n.gettext('Logon Script'),
                        name: 'logonScript',
                        columnWidth: 0.666
                    }], [{
                        xtype: 'extuxclearabledatefield',
                        fieldLabel: this.app.i18n.gettext('Password Can Change'),
                        name: 'pwdCanChange',
                        emptyText: this.app.i18n.gettext('not set')
                    }, {
                        xtype: 'extuxclearabledatefield',
                        fieldLabel: this.app.i18n.gettext('Password Must Change'),
                        name: 'pwdMustChange',
                        emptyText: this.app.i18n.gettext('not set')
                    }, {
                        xtype: 'extuxclearabledatefield',
                        fieldLabel: this.app.i18n.gettext('Kick Off Time'),
                        name: 'kickoffTime',
                        emptyText: this.app.i18n.gettext('not set')
                    }]]
                }]
            }];
        }
        
        return [];
    },
    
    /**
     * Init IMAP tab items
     * 
     * @return {Array} - array of IMAP tab items
     */
    initImap: function () {
        if (Tine.Admin.registry.get('manageImapEmailUser')) {
            return [{
                xtype: 'fieldset',
                title: this.app.i18n.gettext('IMAP Quota (MB)'),
                autoHeight: true,
                checkboxToggle: true,
                layout: 'hfit',
                listeners: {
                    scope: this,
                    collapse: function() {
                        this.getForm().findField('emailMailQuota').setValue(null);
                    }
                },
                items: [{
                    xtype: 'columnform',
                    labelAlign: 'top',
                    formDefaults: {
                        xtype: 'textfield',
                        anchor: '100%',
                        columnWidth: 0.666
                    },
                    items: [[{
                        fieldLabel: this.app.i18n.gettext('Quota'),
                        emptyText: this.app.i18n.gettext('no quota set'),
                        name: 'emailMailQuota',
                        xtype: 'uxspinner',
                        strategy: new Ext.ux.form.Spinner.NumberStrategy({
                            incrementValue : 10,
                            alternateIncrementValue: 50,
                            minValue: 0,
                            allowDecimals : false
                        })
                    }], [{
                        fieldLabel: this.app.i18n.gettext('Current Mailbox size'),
                        name: 'emailMailSize',
                        xtype: 'displayfield',
                        style: this.displayFieldStyle
                    }]]
                }]
            }, {
                xtype: 'fieldset',
                title: this.app.i18n.gettext('Sieve Quota (MB)'),
                autoHeight: true,
                checkboxToggle: true,
                layout: 'hfit',
                listeners: {
                    scope: this,
                    collapse: function() {
                        this.getForm().findField('emailSieveQuota').setValue(null);
                    }
                },
                items: [{
                    xtype: 'columnform',
                    labelAlign: 'top',
                    formDefaults: {
                        xtype: 'textfield',
                        anchor: '100%',
                        columnWidth: 0.666
                    },
                    items: [[{
                        fieldLabel: this.app.i18n.gettext('Quota'),
                        emptyText: this.app.i18n.gettext('no quota set'),
                        name: 'emailSieveQuota',
                        xtype: 'uxspinner',
                        strategy: new Ext.ux.form.Spinner.NumberStrategy({
                            incrementValue : 10,
                            alternateIncrementValue: 50,
                            minValue: 0,
                            allowDecimals : false
                        })
                    }], [{
                        fieldLabel: this.app.i18n.gettext('Current Sieve size'),
                        name: 'emailSieveSize',
                        xtype: 'displayfield',
                        style: this.displayFieldStyle
                    }]
                    ]
                }]
            }, {
                xtype: 'fieldset',
                title: this.app.i18n.gettext('Information'),
                autoHeight: true,
                checkboxToggle: false,
                layout: 'hfit',
                items: [{
                    xtype: 'columnform',
                    labelAlign: 'top',
                    formDefaults: {
                        xtype: 'displayfield',
                        anchor: '100%',
                        columnWidth: 0.666,
                        style: this.displayFieldStyle
                    },
                    items: [[{
                        fieldLabel: this.app.i18n.gettext('Last Login'),
                        name: 'emailLastLogin'
                    }]]
                }]
            }];
        }
        
        return [];
    },
    
    /**
     * @private
     * 
     * init email grids
     * @return Array
     * 
     * TODO     add ctx menu
     */
    initSmtp: function () {
        if (! Tine.Admin.registry.get('manageSmtpEmailUser')) {
            return [];
        }
        
        var commonConfig = {
            autoExpandColumn: 'email',
            quickaddMandatory: 'email',
            frame: false,
            useBBar: true,
            dataField: 'email',
            height: 200,
            columnWidth: 0.5,
            recordClass: Ext.data.Record.create([
                { name: 'email' }
            ])
        };
        
        var smtpConfig = Tine.Felamimail.registry.get('defaults').smtp;
        var domains = (smtpConfig.secondarydomains && smtpConfig.secondarydomains.length) ? smtpConfig.secondarydomains.split(',') : [];
        if (smtpConfig.primarydomain.length) {
            domains.push(smtpConfig.primarydomain);
        }
        var app = this.app,
            record = this.record;
            
        this.aliasesGrid = new Tine.widgets.grid.QuickaddGridPanel(
            Ext.apply({
                onNewentry: function(value) {
                    var domain = value.email.split('@')[1];
                    if (domains.indexOf(domain) > -1) {
                        Tine.widgets.grid.QuickaddGridPanel.prototype.onNewentry.call(this, value);
                    } else {
                        Ext.MessageBox.show({
                            buttons: Ext.Msg.OK,
                            icon: Ext.MessageBox.WARNING,
                            title: app.i18n._('Domain not allowed'),
                            msg: String.format(app.i18n._('The domain {0} of the alias {1} you tried to add is neither configured as primary domain nor set as a secondary domain in the setup.'
                                + ' Please add this domain to the secondary domains in SMTP setup or use another domain which is configured already.'),
                                '<b>' + domain + '</b>', '<b>' + value.email + '</b>')
                        });
                        return false;
                    }
                },
                cm: new Ext.grid.ColumnModel([{
                    id: 'email', 
                    header: this.app.i18n.gettext('Email Alias'), 
                    dataIndex: 'email', 
                    width: 300, 
                    hideable: false, 
                    sortable: true,
                    quickaddField: new Ext.form.TextField({
                        emptyText: this.app.i18n.gettext('Add an alias address...'),
                        vtype: 'email'
                    }),
                    editor: new Ext.form.TextField({allowBlank: false})
                }])
            }, commonConfig)
        );
        this.aliasesGrid.render(document.body);
        
        var aliasesStore = this.aliasesGrid.getStore();

        this.forwardsGrid = new Tine.widgets.grid.QuickaddGridPanel(
            Ext.apply({
                onNewentry: function(value) {
                    if (value.email === record.get('accountEmailAddress') || aliasesStore.find('email', value.email) !== -1) {
                        Ext.MessageBox.show({
                            buttons: Ext.Msg.OK,
                            icon: Ext.MessageBox.WARNING,
                            title: app.i18n._('Forwarding to self'),
                            msg: app.i18n._('You are not allowed to set a forward email address that is identical to the users primary email or one of his aliases.')
                        });
                        return false;
                    } else {
                        Tine.widgets.grid.QuickaddGridPanel.prototype.onNewentry.call(this, value);
                    }
                },
                cm: new Ext.grid.ColumnModel([{
                    id: 'email', 
                    header: this.app.i18n.gettext('Email Forward'), 
                    dataIndex: 'email', 
                    width: 300, 
                    hideable: false, 
                    sortable: true,
                    quickaddField: new Ext.form.TextField({
                        emptyText: this.app.i18n.gettext('Add a forward address...'),
                        vtype: 'email'
                    }),
                    editor: new Ext.form.TextField({allowBlank: false}) 
                }])
            }, commonConfig)
        );
        this.forwardsGrid.render(document.body);
        
        return [
            [this.aliasesGrid, this.forwardsGrid],
            [{hidden: true},
             {
                fieldLabel: this.app.i18n.gettext('Forward Only'),
                name: 'emailForwardOnly',
                xtype: 'checkbox',
                readOnly: false
            }]
        ];
    },
    
    /**
     * @private
     */
    getFormItems: function () {
        this.displayFieldStyle = {
            border: 'silver 1px solid',
            padding: '3px',
            height: '11px'
        };
        
        this.passwordConfirmWindow = new Ext.Window({
            title: this.app.i18n.gettext('Password confirmation'),
            closeAction: 'hide',
            modal: true,
            width: 300,
            height: 150,
            items: [{
                xtype: 'form',
                bodyStyle: 'padding: 5px;',
                buttonAlign: 'right',
                labelAlign: 'top',
                anchor: '100%',
                monitorValid: true,
                defaults: { anchor: '100%' },
                items: [{
                    xtype: 'textfield',
                    inputType: 'password',
                    id: 'passwordRepeat',
                    fieldLabel: this.app.i18n.gettext('Repeat password'), 
                    name: 'passwordRepeat',
                    validator: this.onPasswordConfirm.createDelegate(this),
                    listeners: {
                        scope: this,
                        specialkey: function (field, event) {
                            if (event.getKey() === event.ENTER) {
                                // call OK button handler
                                this.passwordConfirmWindow.items.first().buttons[1].handler.call(this);
                            }
                        }
                    }
                }, {
                    xtype: 'displayfield',
                    hideLabel: true,
                    id: 'passwordStatus',
                    value: this.app.i18n.gettext('Passwords do not match!')
                }],
                buttons: [{
                    text: _('Cancel'),
                    iconCls: 'action_cancel',
                    scope: this,
                    handler: function () {
                        this.passwordConfirmWindow.hide();
                    }
                }, {
                    text: _('Ok'),
                    formBind: true,
                    iconCls: 'action_saveAndClose',
                    scope: this,
                    handler: function () {
                        var confirmForm = this.passwordConfirmWindow.items.first().getForm();
                        
                        // check if confirm form is valid (we need this if special key called button handler)
                        if (confirmForm.isValid()) {
                            this.passwordConfirmWindow.hide();
                            // focus email field
                            this.getForm().findField('accountEmailAddress').focus(true, 100);
                        }
                    }
                }]
            }],
            listeners: {
                scope: this,
                show: function (win) {
                    var confirmForm = this.passwordConfirmWindow.items.first().getForm();
                    
                    confirmForm.reset();
                    confirmForm.findField('passwordRepeat').focus(true, 500);
                }
            }
        });
        this.passwordConfirmWindow.render(document.body);
        
        return {
            xtype: 'tabpanel',
            deferredRender: false,
            border: false,
            plain: true,
            activeTab: 0,
            items: [{
                title: this.app.i18n.gettext('Account'),
                autoScroll: true,
                border: false,
                frame: true,
                layout: 'hfit',
                items: [{
                    xtype: 'columnform',
                    labelAlign: 'top',
                    formDefaults: {
                        xtype: 'textfield',
                        anchor: '100%',
                        labelSeparator: '',
                        columnWidth: 0.333
                    },
                    items: [[{
                        fieldLabel: this.app.i18n.gettext('First name'),
                        name: 'accountFirstName',
                        columnWidth: 0.5,
                        listeners: {
                            render: function (field) {
                                field.focus(false, 250);
                                field.selectText();
                            }
                        }
                    }, {
                        fieldLabel: this.app.i18n.gettext('Last name'),
                        name: 'accountLastName',
                        allowBlank: false,
                        columnWidth: 0.5
                    }], [{
                        fieldLabel: this.app.i18n.gettext('Login name'),
                        name: 'accountLoginName',
                        allowBlank: false,
                        columnWidth: 0.5
                    }, {
                        fieldLabel: this.app.i18n.gettext('Password'),
                        id: 'accountPassword',
                        name: 'accountPassword',
                        inputType: 'password',
                        columnWidth: 0.5,
                        passwordsMatch: true,
                        enableKeyEvents: true,
                        listeners: {
                            scope: this,
                            blur: function (field) {
                                var fieldValue = field.getValue();
                                if (fieldValue !== '') {
                                    // show password confirmation
                                    // NOTE: we can't use Ext.Msg.prompt because field has to be of inputType: 'password'
                                    this.passwordConfirmWindow.show.defer(100, this.passwordConfirmWindow);
                                }
                            },
                            destroy: function () {
                                // destroy password confirm window
                                this.passwordConfirmWindow.destroy();
                            },
                            keydown: function (field) {
                                field.passwordsMatch = false;
                            }
                        },
                        validateValue: function (value) {
                            return this.passwordsMatch;
                        }
                    }], [{
                        vtype: 'email',
                        fieldLabel: this.app.i18n.gettext('Email'),
                        name: 'accountEmailAddress',
                        id: 'accountEmailAddress',
                        columnWidth: 0.5
                    }, {
                        //vtype: 'email',
                        fieldLabel: this.app.i18n.gettext('OpenID'),
                        emptyText: '(' + this.app.i18n.gettext('Login name') + ')',
                        name: 'openid',
                        columnWidth: 0.5
                    }], [{
                        xtype: 'tinerecordpickercombobox',
                        fieldLabel: this.app.i18n.gettext('Primary group'),
                        listWidth: 250,
                        name: 'accountPrimaryGroup',
                        blurOnSelect: true,
                        allowBlank: false,
                        recordClass: Tine.Admin.Model.Group,
                        listeners: {
                            scope: this,
                            'select': function (combo, record, index) {
                                // refresh grid
                                if (this.pickerGridGroups) {
                                    this.pickerGridGroups.getView().refresh();
                                }
                            }
                        }
                    }, {
                        xtype: 'combo',
                        fieldLabel: this.app.i18n.gettext('Status'),
                        name: 'accountStatus',
                        mode: 'local',
                        triggerAction: 'all',
                        allowBlank: false,
                        editable: false,
                        store: [
                            ['enabled',  this.app.i18n.gettext('enabled')],
                            ['disabled', this.app.i18n.gettext('disabled')],
                            ['expired',  this.app.i18n.gettext('expired')],
                            ['blocked',  this.app.i18n.gettext('blocked')]
                        ],
                        listeners: {
                            scope: this,
                            select: function (combo, record) {
                                switch (record.data.field1) {
                                    case 'blocked':
                                        Ext.Msg.alert(this.app.i18n._('Invalid Status'),
                                            this.app.i18n._('Blocked status is only valid if the user tried to login with a wrong password to often. It is not possible to set this status here.'));
                                        combo.setValue(combo.startValue);
                                        break;
                                    case 'expired':
                                        this.getForm().findField('accountExpires').setValue(new Date());
                                        break;
                                    case 'enabled':
                                        var expiryDateField = this.getForm().findField('accountExpires'),
                                            expiryDate = expiryDateField.getValue(),
                                            now = new Date();
                                            
                                        if (expiryDate < now) {
                                            expiryDateField.setValue('');
                                        }
                                        break;
                                    default:
                                        // do nothing
                                }
                            }
                        }
                    }, {
                        xtype: 'extuxclearabledatefield',
                        fieldLabel: this.app.i18n.gettext('Expires'),
                        name: 'accountExpires',
                        emptyText: this.app.i18n.gettext('never')
                    }], [{
                        xtype: 'combo',
                        fieldLabel: this.app.i18n.gettext('Visibility'),
                        name: 'visibility',
                        mode: 'local',
                        triggerAction: 'all',
                        allowBlank: false,
                        editable: false,
                        store: [['displayed', this.app.i18n.gettext('Display in addressbook')], ['hidden', this.app.i18n.gettext('Hide from addressbook')]],
                        listeners: {
                            scope: this,
                            select: function (combo, record) {
                                // disable container_id combo if hidden
                                var addressbookContainerCombo = this.getForm().findField('container_id');
                                addressbookContainerCombo.setDisabled(record.data.field1 === 'hidden');
                                if (addressbookContainerCombo.getValue() === '') {
                                    addressbookContainerCombo.setValue(null);
                                }
                            }
                        }
                    }, {
                        xtype: 'tinerecordpickercombobox',
                        fieldLabel: this.app.i18n.gettext('Saved in Addressbook'),
                        name: 'container_id',
                        blurOnSelect: true,
                        allowBlank: false,
                        forceSelection: true,
                        listWidth: 250,
                        recordClass: Tine.Tinebase.Model.Container,
                        disabled: this.record.get('visibility') === 'hidden',
                        recordProxy: Tine.Admin.sharedAddressbookBackend
                    }]] 
                }, {
                    xtype: 'fieldset',
                    title: this.app.i18n.gettext('Information'),
                    autoHeight: true,
                    checkboxToggle: false,
                    layout: 'hfit',
                    items: [{
                        xtype: 'columnform',
                        labelAlign: 'top',
                        formDefaults: {
                            xtype: 'displayfield',
                            anchor: '100%',
                            labelSeparator: '',
                            columnWidth: 0.333,
                            style: this.displayFieldStyle
                        },
                        items: [[{
                            fieldLabel: this.app.i18n.gettext('Last login at'),
                            name: 'accountLastLogin',
                            emptyText: this.ldapBackend ? this.app.i18n.gettext("don't know") : this.app.i18n.gettext('never logged in')
                        }, {
                            fieldLabel: this.app.i18n.gettext('Last login from'),
                            name: 'accountLastLoginfrom',
                            emptyText: this.ldapBackend ? this.app.i18n.gettext("don't know") : this.app.i18n.gettext('never logged in')
                        }, {
                            fieldLabel: this.app.i18n.gettext('Password set'),
                            name: 'accountLastPasswordChange',
                            emptyText: this.app.i18n.gettext('never')
                        }]]
                    }]
                }]
            }, {
                title: this.app.i18n.gettext('User groups'),
                border: false,
                frame: true,
                layout: 'fit',
                items: this.initUserGroups()
            }, {
                title: this.app.i18n.gettext('User roles'),
                border: false,
                frame: true,
                layout: 'fit',
                items: this.initUserRoles()
            }, {
                title: this.app.i18n.gettext('Fileserver'),
                disabled: !this.ldapBackend,
                border: false,
                frame: true,
                items: this.initFileserver()
            }, {
                title: this.app.i18n.gettext('IMAP'),
                disabled: ! Tine.Admin.registry.get('manageImapEmailUser'),
                autoScroll: true,
                border: false,
                frame: true,
                layout: 'hfit',
                items: this.initImap()
            }, {
                xtype: 'columnform',
                title: this.app.i18n.gettext('SMTP'),
                disabled: ! Tine.Admin.registry.get('manageSmtpEmailUser'),
                border: false,
                frame: true,
                labelAlign: 'top',
                formDefaults: {
                    xtype: 'textfield',
                    anchor: '100%',
                    labelSeparator: '',
                    columnWidth: 0.5,
                    readOnly: true
                },
                items: this.initSmtp()
            }]
        };
    }
});

/**
 * User Edit Popup
 * 
 * @param   {Object} config
 * @return  {Ext.ux.Window}
 */
Tine.Admin.UserEditDialog.openWindow = function (config) {
    var id = (config.record && config.record.id) ? config.record.id : 0;
    var window = Tine.WindowFactory.getWindow({
        width: 600,
        height: 400,
        name: Tine.Admin.UserEditDialog.prototype.windowNamePrefix + id,
        contentPanelConstructor: 'Tine.Admin.UserEditDialog',
        contentPanelConstructorConfig: config
    });
    return window;
};
/*
 * Tine 2.0
 * 
 * @package     Admin
 * @subpackage  User
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 * TODO         refactor this (don't use Ext.getCmp, extend generic grid panel, ...)
 */

/*global Ext, Tine, Locale*/

Ext.ns('Tine.Admin.Groups');

/*********************************** MAIN DIALOG ********************************************/

Tine.Admin.Groups.Main = {
        
    // references to crated toolbar and grid panel
    groupsToolbar: null,
    gridPanel: null,
        
    actions: {
        addGroup: null,
        editGroup: null,
        deleteGroup: null
    },
    
    handlers: {
        /**
         * onclick handler for addBtn
         */
        addGroup: function (button, event) {
            this.openEditWindow(null);
        },

        /**
         * onclick handler for editBtn
         */
        editGroup: function (button, event) {
            var selectedRows = Ext.getCmp('AdminGroupsGrid').getSelectionModel().getSelections();
            this.openEditWindow(selectedRows[0]);
        },

        
        /**
         * onclick handler for deleteBtn
         */
        deleteGroup: function (button, event) {
            Ext.MessageBox.confirm(this.translation.gettext('Confirm'), this.translation.gettext('Do you really want to delete the selected groups?'), function (button) {
                if (button === 'yes') {
                
                    var groupIds = [],
                        selectedRows = Ext.getCmp('AdminGroupsGrid').getSelectionModel().getSelections();
                        
                    for (var i = 0; i < selectedRows.length; ++i) {
                        groupIds.push(selectedRows[i].id);
                    }
                    
                    Ext.Ajax.request({
                        url: 'index.php',
                        params: {
                            method: 'Admin.deleteGroups',
                            groupIds: groupIds
                        },
                        scope: this,
                        text: this.translation.gettext('Deleting group(s)...'),
                        success: function (result, request) {
                            Ext.getCmp('AdminGroupsGrid').getStore().reload();
                        },
                        failure: function (result, request) {
                            Ext.MessageBox.alert(this.translation.gettext('Failed'), this.translation.gettext('Some error occurred while trying to delete the group.'));
                        }
                    });
                }
            }, this);
        }    
    },
    
    /**
     * open edit window
     * 
     * @param {} record
     */
    openEditWindow: function (record) {
        var popupWindow = Tine.Admin.Groups.EditDialog.openWindow({
            group: record,
            listeners: {
                scope: this,
                'update': function (record) {
                    this.reload();
                }
            }                
        });
    },
    
    initComponent: function () {
        this.translation = new Locale.Gettext();
        this.translation.textdomain('Admin');
        
        this.actions.addGroup = new Ext.Action({
            text: this.translation.gettext('Add Group'),
            disabled: true,
            handler: this.handlers.addGroup,
            iconCls: 'action_addGroup',
            scope: this
        });
        
        this.actions.editGroup = new Ext.Action({
            text: this.translation.gettext('Edit Group'),
            disabled: true,
            handler: this.handlers.editGroup,
            iconCls: 'action_edit',
            scope: this
        });
        
        this.actions.deleteGroup = new Ext.Action({
            text: this.translation.gettext('Delete Group'),
            disabled: true,
            handler: this.handlers.deleteGroup,
            iconCls: 'action_delete',
            scope: this
        });

    },
    
    displayGroupsToolbar: function () {
        
        // if toolbar was allready created set active toolbar and return
        if (this.groupsToolbar) {
            Tine.Tinebase.MainScreen.setActiveToolbar(this.groupsToolbar, true);
            return;
        }
        
        var GroupsAdminQuickSearchField = new Ext.ux.SearchField({
            id: 'GroupsAdminQuickSearchField',
            width: 240,
            emptyText: Tine.Tinebase.translation._hidden('enter searchfilter')
        });
        GroupsAdminQuickSearchField.on('change', function () {
            Ext.getCmp('AdminGroupsGrid').getStore().load({
                params: {
                    start: 0,
                    limit: 50
                }
            });
        }, this);
        
        this.groupsToolbar = new Ext.Toolbar({
            id: 'AdminGroupsToolbar',
            split: false,
            //height: 26,
            items: [{
                xtype: 'buttongroup',
                columns: 5,
                items: [
                    Ext.apply(new Ext.Button(this.actions.addGroup), {
                        scale: 'medium',
                        rowspan: 2,
                        iconAlign: 'top'
                    }), {xtype: 'tbspacer', width: 10},
                    Ext.apply(new Ext.Button(this.actions.editGroup), {
                        scale: 'medium',
                        rowspan: 2,
                        iconAlign: 'top'
                    }), {xtype: 'tbspacer', width: 10},
                    Ext.apply(new Ext.Button(this.actions.deleteGroup), {
                        scale: 'medium',
                        rowspan: 2,
                        iconAlign: 'top'
                    })
                ]
            }, '->', 
                this.translation.gettext('Search:'), 
                ' ',
                GroupsAdminQuickSearchField
            ]
        });

        Tine.Tinebase.MainScreen.setActiveToolbar(this.groupsToolbar, true);
    },

    displayGroupsGrid: function () {
        
        // if grid panel was allready created set active content panel and return
        if (this.gridPanel)    {
            Tine.Tinebase.MainScreen.setActiveContentPanel(this.gridPanel, true);
            return;
        }
        
        if (Tine.Tinebase.common.hasRight('manage', 'Admin', 'accounts')) {
            this.actions.addGroup.setDisabled(false);
        }

        // the datastore
        var dataStore = new Ext.data.JsonStore({
            baseParams: {
                method: 'Admin.getGroups'
            },
            root: 'results',
            totalProperty: 'totalcount',
            id: 'id',
            fields: Tine.Admin.Model.Group,
            // turn on remote sorting
            remoteSort: true
        });
        
        dataStore.setDefaultSort('id', 'asc');

        dataStore.on('beforeload', function (dataStore, options) {
            options = options || {};
            options.params = options.params || {};
            options.params.filter = Ext.getCmp('GroupsAdminQuickSearchField').getValue();
        }, this);
        
        // the paging toolbar
        var pagingToolbar = new Ext.PagingToolbar({
            pageSize: 25,
            store: dataStore,
            displayInfo: true,
            displayMsg: this.translation.gettext('Displaying groups {0} - {1} of {2}'),
            emptyMsg: this.translation.gettext("No groups to display")
        });
        
        // the columnmodel
        var columnModel = new Ext.grid.ColumnModel({
            defaults: {
                sortable: true,
                resizable: true
            },
            columns: [
                { id: 'id', header: this.translation.gettext('ID'), dataIndex: 'id', width: 50, hidden: true },
                { id: 'name', header: this.translation.gettext('Name'), dataIndex: 'name', width: 50 },
                { id: 'description', header: this.translation.gettext('Description'), dataIndex: 'description' }
            ]
        });
        
        // the rowselection model
        var rowSelectionModel = new Ext.grid.RowSelectionModel({multiSelect: true});

        rowSelectionModel.on('selectionchange', function (selectionModel) {
            var rowCount = selectionModel.getCount();

            if (Tine.Tinebase.common.hasRight('manage', 'Admin', 'accounts')) {
                if (rowCount < 1) {
                    // no row selected
                    this.actions.deleteGroup.setDisabled(true);
                    this.actions.editGroup.setDisabled(true);
                } else if (rowCount > 1) {
                    // more than one row selected
                    this.actions.deleteGroup.setDisabled(false);
                    this.actions.editGroup.setDisabled(true);
                } else {
                    // only one row selected
                    this.actions.deleteGroup.setDisabled(false);
                    this.actions.editGroup.setDisabled(false);
                }
            }
        }, this);
        
        // the gridpanel
        this.gridPanel = new Ext.grid.GridPanel({
            id: 'AdminGroupsGrid',
            store: dataStore,
            cm: columnModel,
            tbar: pagingToolbar,     
            autoSizeColumns: false,
            selModel: rowSelectionModel,
            enableColLock: false,
            autoExpandColumn: 'n_family',
            border: false,
            view: new Ext.grid.GridView({
                autoFill: true,
                forceFit: true,
                ignoreAdd: true,
                emptyText: this.translation.gettext('No groups to display')
            }),
            enableHdMenu: false,
            plugins: [new Ext.ux.grid.GridViewMenuPlugin()]
        });
        
        this.gridPanel.on('rowcontextmenu', function (grid, rowIndex, eventObject) {
            eventObject.stopEvent();
            if (! grid.getSelectionModel().isSelected(rowIndex)) {
                grid.getSelectionModel().selectRow(rowIndex);
            }
            
            if (! this.contextMenu) {
                this.contextMenu = new Ext.menu.Menu({
                    id: 'ctxMenuGroups', 
                    items: [
                        this.actions.editGroup,
                        this.actions.deleteGroup,
                        '-',
                        this.actions.addGroup 
                    ]
                });
            }
            this.contextMenu.showAt(eventObject.getXY());
        }, this);
        
        this.gridPanel.on('rowdblclick', function (gridPar, rowIndexPar, ePar) {
            if (Tine.Tinebase.common.hasRight('manage', 'Admin', 'accounts')) {
                var record = gridPar.getStore().getAt(rowIndexPar);
                this.openEditWindow(record);
            }
        }, this);

        // add the grid to the layout
        Tine.Tinebase.MainScreen.setActiveContentPanel(this.gridPanel, true);
    },
    
    /**
     * update datastore with node values and load datastore
     */
    loadData: function () {
        var dataStore = Ext.getCmp('AdminGroupsGrid').getStore();
        dataStore.load({ params: { start: 0, limit: 50 } });
    },

    show: function () {
        if (this.groupsToolbar === null || this.gridPanel === null) {
            this.initComponent();
        }

        this.displayGroupsToolbar();
        this.displayGroupsGrid();

        this.loadData();
    },
    
    reload: function () {
        if (Ext.ComponentMgr.all.containsKey('AdminGroupsGrid')) {
            setTimeout("Ext.getCmp('AdminGroupsGrid').getStore().reload()", 200);
        }
    }
};
/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2007-2012 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 * TODO         refactor this (use more methods from Tine.widgets.dialog.EditRecord)
 */

/*global Ext, Tine, Locale*/

Ext.ns('Tine.Admin.Groups');

/**
 * @namespace   Tine.Admin.Groups
 * @class       Tine.Admin.Groups.EditDialog
 * @extends     Tine.widgets.dialog.EditRecord
 */
Tine.Admin.Groups.EditDialog = Ext.extend(Tine.widgets.dialog.EditRecord, {
    /**
     * var group
     */
    group: null,

    windowNamePrefix: 'groupEditWindow_',
    
    id: 'groupDialog',
    layout: 'fit',
    border: false,
    labelWidth: 120,
    labelAlign: 'top',
    
    handlerApplyChanges: function(button, event, closeWindow) {
        var form = this.getForm();
        
        if (form.isValid()) {
            Ext.MessageBox.wait(this.translation.gettext('Please wait'), this.translation.gettext('Updating Memberships'));
            
            // get group members
            var groupMembers = [];
            this.membersStore.each(function (record) {
                groupMembers.push(record.id);
            });
            
            // update record with form data               
            form.updateRecord(this.group);

            /*********** save group members & form ************/
            
            Ext.Ajax.request({
                params: {
                    method: 'Admin.saveGroup', 
                    groupData: this.group.data,
                    groupMembers: groupMembers
                },
                success: function (response) {
                    /*
                    if(window.opener.Tine.Admin.Groups) {
                        window.opener.Tine.Admin.Groups.Main.reload();
                    }
                    */
                    this.fireEvent('update', Ext.util.JSON.encode(this.group.data));
                    if (closeWindow === true) {
                        //window.close();
                        this.window.close();
                    } else {
                        this.onRecordLoad(response);
                    }
                    Ext.MessageBox.hide();
                },
                failure: function (result, request) {
                    Ext.MessageBox.alert(this.translation.gettext('Failed'), this.translation.gettext('Could not save group.'));
                },
                scope: this 
            });
        } else {
            Ext.MessageBox.alert(this.translation.gettext('Errors'), this.translation.gettext('Please fix the errors noted.'));
        }
    },
    
    /**
     * function updateRecord
     */
    updateRecord: function (groupData) {
        // if groupData is empty (=array), set to empty object because array won't work!
        if (groupData.length === 0) {
            groupData = {};
        }
        this.group = new Tine.Admin.Model.Group(groupData, groupData.id ? groupData.id : 0);
        
        // tweak, as group members are not in standard form cycle yet
        this.membersStore.loadData(this.group.get('groupMembers'));
    },

    /**
     * function updateToolbarButtons
     * 
     */
    updateToolbarButtons: function (rights) {
    },
    
    /**
     * function getFormContents
     * 
     */
    getFormContents: function () {
        var editGroupDialog = {
            layout: 'border',
            border: false,
            width: 600,
            height: 500,
            items: [{
                region: 'north',
                xtype: 'columnform',
                border: false,
                autoHeight: true,
                items: [[{
                    columnWidth: 1,
                    xtype: 'textfield',
                    fieldLabel: this.translation.gettext('Group Name'), 
                    name: 'name',
                    anchor: '100%',
                    allowBlank: false
                }], [{
                    columnWidth: 1,
                    xtype: 'textarea',
                    name: 'description',
                    fieldLabel: this.translation.gettext('Description'),
                    grow: false,
                    preventScrollbars: false,
                    anchor: '100%',
                    height: 60
                }], [{
                    columnWidth: 0.5,
                    xtype: 'combo',
                    fieldLabel: this.translation.gettext('Visibility'),
                    name: 'visibility',
                    mode: 'local',
                    triggerAction: 'all',
                    allowBlank: false,
                    editable: false,
                    store: [['displayed', this.translation.gettext('Display in addressbook')], ['hidden', this.translation.gettext('Hide from addressbook')]],
                    listeners: {
                        scope: this,
                        select: function (combo, record) {
                            // disable container_id combo if hidden
                            this.getForm().findField('container_id').setDisabled(record.data.field1 === 'hidden');
                            if(record.data.field1 === 'hidden') {
                                this.getForm().findField('container_id').clearInvalid();
                            } else {
                                this.getForm().findField('container_id').isValid();
                            }
                        }
                    }
                }, {
                    columnWidth: 0.5,
                    xtype: 'tinerecordpickercombobox',
                    fieldLabel: this.translation.gettext('Saved in Addressbook'),
                    name: 'container_id',
                    blurOnSelect: true,
                    allowBlank: false,
                    listWidth: 250,
                    recordClass: Tine.Tinebase.Model.Container,
                    recordProxy: Tine.Admin.sharedAddressbookBackend,
                    disabled: this.group.get('visibility') === 'hidden'
                }]]
            }, {
                xtype: 'tinerecordpickergrid',
                title: this.translation.gettext('Group Members'),
                store: this.membersStore,
                region: 'center',
                anchor: '100% 100%',
                showHidden: true
            }]
        };
        
        return editGroupDialog;
    },
    
    initComponent: function () {
        this.translation = new Locale.Gettext();
        this.translation.textdomain('Admin');
        
        this.group = this.group ? this.group : new Tine.Admin.Model.Group({}, 0);
        
        if (this.group.id !== 0) {
            Ext.Ajax.request({
                scope: this,
                success: this.onRecordLoad,
                params: {
                    method: 'Admin.getGroup',
                    groupId: this.group.id
                }
            });
        } else {
            this.group = new Tine.Admin.Model.Group(Tine.Admin.Model.Group.getDefaultData(), 0);
        }
                
        this.membersStore = new Ext.data.JsonStore({
            root: 'results',
            totalProperty: 'totalcount',
            id: 'id',
            fields: Tine.Tinebase.Model.Account
        });
        
        this.items = this.getFormContents();
        Tine.Admin.Groups.EditDialog.superclass.initComponent.call(this);
    },
    
    onRender: function (ct, position) {
        Tine.widgets.dialog.EditDialog.superclass.onRender.call(this, ct, position);
        
        // generalized keybord map for edit dlgs
        var map = new Ext.KeyMap(this.el, [{
            key: [10, 13], // ctrl + return
            ctrl: true,
            fn: this.handlerApplyChanges.createDelegate(this, [true], true),
            scope: this
        }]);

        this.loadMask = new Ext.LoadMask(ct, {msg: String.format(_('Transferring {0}...'), this.translation.gettext('Group'))});
        
        if (this.group.id !== 0) {
            this.loadMask.show();
        } else {
            this.window.setTitle(this.translation.gettext('Add new group'));
            this.getForm().loadRecord(this.group);
        }
    },
    
    onRecordLoad: function (response) {
        this.getForm().findField('name').focus(false, 350);
        var recordData = Ext.util.JSON.decode(response.responseText);
        this.updateRecord(recordData);

        if (! this.group.id) {
            this.window.setTitle(this.translation.gettext('Add new group'));
        } else {
            this.window.setTitle(String.format(this.translation.gettext('Edit Group "{0}"'), this.group.get('name')));
        }

        this.getForm().loadRecord(this.group);
        this.updateToolbarButtons();
        
        this.loadMask.hide();
    }
});


/**
 * Groups Edit Popup
 */
Tine.Admin.Groups.EditDialog.openWindow = function (config) {
    config.group = config.group ? config.group : new Tine.Admin.Model.Group({}, 0);
    var window = Tine.WindowFactory.getWindow({
        width: 400,
        height: 600,
        name: Tine.Admin.Groups.EditDialog.prototype.windowNamePrefix + config.group.id,
        contentPanelConstructor: 'Tine.Admin.Groups.EditDialog',
        contentPanelConstructorConfig: config
    });
    return window;
};
/**
 * Tine 2.0
 * 
 * @package     Admin
 * @subpackage  AccessLog
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philip Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2010-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */

Ext.ns('Tine.Admin.accessLog');

/**
 * AccessLog 'mainScreen'
 * 
 * @static
 */
Tine.Admin.accessLog.show = function () {
    var app = Tine.Tinebase.appMgr.get('Admin');
    if (! Tine.Admin.accessLog.gridPanel) {
        Tine.Admin.accessLog.gridPanel = new Tine.Admin.accessLog.GridPanel({
            app: app
        });
    }
    else {
        Tine.Admin.accessLog.gridPanel.loadGridData.defer(100, Tine.Admin.accessLog.gridPanel, []);
    }
    
    Tine.Tinebase.MainScreen.setActiveContentPanel(Tine.Admin.accessLog.gridPanel, true);
    Tine.Tinebase.MainScreen.setActiveToolbar(Tine.Admin.accessLog.gridPanel.actionToolbar, true);
};

/**
 * AccessLog grid panel
 * 
 * @namespace   Tine.Admin.accessLog
 * @class       Tine.Admin.accessLog.GridPanel
 * @extends     Tine.widgets.grid.GridPanel
 */
Tine.Admin.accessLog.GridPanel = Ext.extend(Tine.widgets.grid.GridPanel, {
    
    recordClass: Tine.Admin.Model.AccessLog,
    recordProxy: Tine.Admin.accessLogBackend,
    defaultSortInfo: {field: 'li', direction: 'DESC'},
    evalGrants: false,
    gridConfig: {
        id: 'gridAdminAccessLogs',
        autoExpandColumn: 'login_name'
    },
    
    initComponent: function() {
        this.gridConfig.columns = this.getColumns();
        this.initFilterToolbar();
        
        this.plugins = this.plugins || [];
        this.plugins.push(this.filterToolbar);
            
        Tine.Admin.accessLog.GridPanel.superclass.initComponent.call(this);
    },
    
    /**
     * init actions with actionToolbar, contextMenu and actionUpdater
     * 
     * @private
     */
    initActions: function() {
        
        this.initDeleteAction();
        
        this.actionUpdater.addActions([
            this.action_deleteRecord
        ]);
        
        this.actionToolbar = new Ext.Toolbar({
            items: [{
                xtype: 'buttongroup',
                columns: 1,
                items: [
                    Ext.apply(new Ext.Button(this.action_deleteRecord), {
                        scale: 'medium',
                        rowspan: 2,
                        iconAlign: 'top',
                        arrowAlign:'right'
                    })
                ]}
             ]
        });
        
        if (this.filterToolbar && typeof this.filterToolbar.getQuickFilterField == 'function') {
            this.actionToolbar.add('->', this.filterToolbar.getQuickFilterField());
        }
        
        this.contextMenu = new Ext.menu.Menu({
            items: [this.action_deleteRecord]
        });
    },
    
    /**
     * initialises filter toolbar
     */
    initFilterToolbar: function() {
        this.filterToolbar = new Tine.widgets.grid.FilterToolbar({
            filterModels: [
                {label: this.app.i18n._('Access Log'),  field: 'query',         operators: ['contains']},
                {label: this.app.i18n._('IP Address'),  field: 'ip'},
                {label: this.app.i18n._('User'),        field: 'account_id',    valueType: 'user'},
                {label: this.app.i18n._('Login Time'),  field: 'li',            valueType: 'date', pastOnly: true        },
                {label: this.app.i18n._('Logout Time'), field: 'lo',            valueType: 'date', pastOnly: true        },
                {label: this.app.i18n._('Client Type'), field: 'clienttype'}
            ],
            defaultFilter: 'query',
            filters: [
                {field: 'li',           operator: 'within', value: 'weekThis'},
                {field: 'clienttype',   operator: 'equals', value: 'TineJson'}
            ],
            plugins: [
                new Tine.widgets.grid.FilterToolbarQuickFilterPlugin()
            ]
        });
    },
    
    /**
     * returns cm
     * @private
     */
    getColumns: function(){
        return [
            { header: this.app.i18n._('Session ID'), id: 'sessionid', dataIndex: 'sessionid', width: 200, hidden: true},
            { header: this.app.i18n._('Login Name'), id: 'login_name', dataIndex: 'login_name'},
            { header: this.app.i18n._('Name'), id: 'account_id', dataIndex: 'account_id', width: 170, sortable: false, renderer: Tine.Tinebase.common.usernameRenderer},
            { header: this.app.i18n._('IP Address'), id: 'ip', dataIndex: 'ip', width: 150},
            { header: this.app.i18n._('Login Time'), id: 'li', dataIndex: 'li', width: 140, renderer: Tine.Tinebase.common.dateTimeRenderer},
            { header: this.app.i18n._('Logout Time'), id: 'lo', dataIndex: 'lo', width: 140, renderer: Tine.Tinebase.common.dateTimeRenderer},
            { header: this.app.i18n._('Result'), id: 'result', dataIndex: 'result', width: 110, renderer: this.resultRenderer, scope: this},
            { header: this.app.i18n._('Client Type'), id: 'clienttype', dataIndex: 'clienttype', width: 50}
        ];
    },
    
    /**
     * result renderer
     * 
     * @param {} _value
     * @param {} _cellObject
     * @param {} _record
     * @param {} _rowIndex
     * @param {} _colIndex
     * @param {} _dataStore
     * @return String
     */
    resultRenderer: function(_value, _cellObject, _record, _rowIndex, _colIndex, _dataStore) {
        var gridValue;
        
        switch (_value) {
            case '-102' :
                gridValue = this.app.i18n._('user blocked');
                break;

            case '-101' :
                gridValue = this.app.i18n._('password expired');
                break;

            case '-100' :
                gridValue = this.app.i18n._('user disabled');
                break;

            case '-3' :
                gridValue = this.app.i18n._('invalid password');
                break;

            case '-2' :
                gridValue = this.app.i18n._('ambiguous username');
                break;

            case '-1' :
                gridValue = this.app.i18n._('user not found');
                break;

            case '0' :
                gridValue = this.app.i18n._('failure');
                break;

            case '1' :
                gridValue = this.app.i18n._('success');
                break;
        }
        
        return gridValue;
    }
});
/*
 * Tine 2.0
 * 
 * @package     Admin
 * @subpackage  sambaMachine
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2009 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */

Ext.ns('Tine.Admin.Model');

Tine.Admin.Model.sambaMachineArray = [
    { name: 'accountId'            },
    { name: 'accountLoginName'     },
    { name: 'accountLastName'      },
    { name: 'accountFullName'      },
    { name: 'accountDisplayName'   },
    { name: 'accountPrimaryGroup'  },
    { name: 'accountHomeDirectory' },
    { name: 'accountLoginShell'    },
    { name: 'sid'                  },
    { name: 'primaryGroupSID'      },
    { name: 'acctFlags'            },
    //{ name: 'logonTime',     type: 'date', dateFormat: Date.patterns.ISO8601Long },
    //{ name: 'logoffTime',    type: 'date', dateFormat: Date.patterns.ISO8601Long },
    //{ name: 'kickoffTime',   type: 'date', dateFormat: Date.patterns.ISO8601Long },
    //{ name: 'pwdLastSet',    type: 'date', dateFormat: Date.patterns.ISO8601Long },
    //{ name: 'pwdCanChange',  type: 'date', dateFormat: Date.patterns.ISO8601Long },
    { name: 'pwdMustChange', type: 'date', dateFormat: Date.patterns.ISO8601Long }
];

Tine.Admin.Model.SambaMachine = Tine.Tinebase.data.Record.create(Tine.Admin.Model.sambaMachineArray, {
    appName: 'Admin',
    modelName: 'SambaMachine',
    idProperty: 'accountId',
    titleProperty: 'accountDisplayName',
    // ngettext('Computer', 'Computers', n);
    recordName: 'Computer',
    recordsName: 'Computers'
});

Tine.Admin.sambaMachineBackend = new Tine.Tinebase.data.RecordProxy({
    appName: 'Admin',
    modelName: 'SambaMachine',
    recordClass: Tine.Admin.Model.SambaMachine,
    idProperty: 'accountId'
});
/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2009-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 */
Ext.ns('Tine.Admin.sambaMachine');

/**
 * Samba machine 'mainScreen'
 */
Tine.Admin.sambaMachine.show = function() {
    var app = Tine.Tinebase.appMgr.get('Admin');
    if (! Tine.Admin.sambaMachine.gridPanel) {
        Tine.Admin.sambaMachine.gridPanel = new Tine.Admin.SambaMachineGridPanel({
            app: app
        });
    } else {
        setTimeout ("Ext.getCmp('gridAdminComputers').getStore().load({ params: { start:0, limit:50 } })", 100);
    }
    
    Tine.Tinebase.MainScreen.setActiveContentPanel(Tine.Admin.sambaMachine.gridPanel, true);
    Tine.Tinebase.MainScreen.setActiveToolbar(Tine.Admin.sambaMachine.gridPanel.actionToolbar, true);
};

/**
 * SambaMachine grid panel
 *
 * @namespace   Tine.Admin.sambaMachine
 * @class       Tine.Admin.SambaMachineGridPanel
 * @extends     Tine.widgets.grid.GridPanel
 */
Tine.Admin.SambaMachineGridPanel = Ext.extend(Tine.widgets.grid.GridPanel, {
    // model generics
    recordClass: Tine.Admin.Model.SambaMachine,
    recordProxy: Tine.Admin.sambaMachineBackend,
    defaultSortInfo: {field: 'accountLoginName', direction: 'ASC'},
    evalGrants: false,
    gridConfig: {
        id: 'gridAdminComputers',
        autoExpandColumn: 'accountDisplayName'
    },
    
    initComponent: function() {
        this.gridConfig.columns = this.getColumns();
        this.initFilterToolbar();
        
        this.plugins = this.plugins || [];
        this.plugins.push(this.filterToolbar);
        
        Tine.Admin.SambaMachineGridPanel.superclass.initComponent.call(this);
    },
    
    /**
     * initialises filter toolbar
     */
    initFilterToolbar: function() {
        this.filterToolbar = new Tine.widgets.grid.FilterToolbar({
            filterModels: [
                {label: this.app.i18n._('Computer Name'),    field: 'query',       operators: ['contains']}
                //{label: this.app.i18n._('Description'),    field: 'description', operators: ['contains']},
            ],
            defaultFilter: 'query',
            filters: [],
            plugins: [
                new Tine.widgets.grid.FilterToolbarQuickFilterPlugin()
            ]
        });
    },    
    
    /**
     * returns cm
     * @private
     */
    getColumns: function(){
        return [{
            id: 'accountId',
            header: this.app.i18n._("ID"),
            width: 100,
            sortable: true,
            dataIndex: 'accountId',
            hidden: true
        },{
            id: 'accountLoginName',
            header: this.app.i18n._("Name"),
            width: 350,
            sortable: true,
            dataIndex: 'accountLoginName'
        },{
            id: 'accountDisplayName',
            header: this.app.i18n._("Description"),
            width: 350,
            sortable: true,
            dataIndex: 'accountDisplayName'
        }];
    }
});
/*
 * Tine 2.0
 * 
 * @package     Admin
 * @subpackage  SambaMachine
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2009 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */

Ext.ns('Tine.Admin.sambaMachine');

/**
 * @namespace   Tine.Admin.sambaMachine
 * @class       Tine.Admin.SambaMachineEditDialog
 * @extends     Tine.widgets.dialog.EditDialog
 */
Tine.Admin.SambaMachineEditDialog  = Ext.extend(Tine.widgets.dialog.EditDialog, {
    
    /**
     * @private
     */
    windowNamePrefix: 'sambaMachineEditWindow_',
    appName: 'Admin',
    recordClass: Tine.Admin.Model.SambaMachine,
    recordProxy: Tine.Admin.sambaMachineBackend,
    evalGrants: false,
    
    getFormItems: function() {
        return {
            xtype: 'columnform',
            labelAlign: 'top',
            border: false,
            formDefaults: {
                xtype:'textfield',
                anchor: '100%',
                labelSeparator: '',
                columnWidth: 1
            },
            items: [[{
                fieldLabel: this.app.i18n._('Computer Name'),
                name: 'accountLoginName'
            }]]
        };
    }
});

/**
 * User edit popup
 */
Tine.Admin.SambaMachineEditDialog.openWindow = function (config) {
    var id = (config.record && config.record.id) ? config.record.id : 0;
    var window = Tine.WindowFactory.getWindow({
        width: 300,
        height: 100,
        name: Tine.Admin.SambaMachineEditDialog.windowNamePrefix + id,
        contentPanelConstructor: 'Tine.Admin.SambaMachineEditDialog',
        contentPanelConstructorConfig: config
    });
    return window;
};
/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 * TODO         split into two files (grid & edit dlg)
 * TODO         fix autoheight in edit dlg (use border layout?)
 */
 
/*global Ext, Tine, Locale*/

Ext.ns('Tine.Admin.Tags');

Tine.Admin.Tags.Main = {
    
    //  references to created toolbar and grid panel
    tagsToolbar: null,
    gridPanel: null,
    
    actions: {
        addTag: null,
        editTag: null,
        deleteTag: null
    },
    
    handlers: {
        /**
         * onclick handler for addBtn
         */
        addTag: function (button, event) {
            Tine.Admin.Tags.EditDialog.openWindow({
                tag: null,
                listeners: {
                    scope: this,
                    'update': function (record) {
                        this.reload();
                    }
                }
            });
        },

        /**
         * onclick handler for editBtn
         */
        editTag: function (button, event) {
            var selectedRows = Ext.getCmp('AdminTagsGrid').getSelectionModel().getSelections();
            Tine.Admin.Tags.EditDialog.openWindow({
                tag: selectedRows[0],
                listeners: {
                    scope: this,
                    'update': function (record) {
                        this.reload();
                    }
                }
            });
        },
        
        /**
         * onclick handler for deleteBtn
         */
        deleteTag: function (button, event) {
            Ext.MessageBox.confirm(this.translation.gettext('Confirm'), this.translation.gettext('Do you really want to delete the selected tags?'), function (button) {
                if (button === 'yes') {
                
                    var tagIds = [],
                        selectedRows = Ext.getCmp('AdminTagsGrid').getSelectionModel().getSelections();
                        
                    for (var i = 0; i < selectedRows.length; ++i) {
                        tagIds.push(selectedRows[i].id);
                    }
                    
                    tagIds = tagIds;
                    
                    Ext.Ajax.request({
                        url: 'index.php',
                        params: {
                            method: 'Admin.deleteTags',
                            tagIds: tagIds
                        },
                        text: this.translation.gettext('Deleting tag(s)...'),
                        success: function (result, request) {
                            Ext.getCmp('AdminTagsGrid').getStore().reload();
                        }
                    });
                }
            }, this);
        }    
    },
    
    initComponent: function () {
        this.translation = new Locale.Gettext();
        this.translation.textdomain('Admin');
        
        this.actions.addTag = new Ext.Action({
            text: this.translation.gettext('Add Tag'),
            handler: this.handlers.addTag,
            iconCls: 'action_tag',
            scope: this,
            disabled: !(Tine.Tinebase.common.hasRight('manage', 'Admin', 'shared_tags'))
        });
        
        this.actions.editTag = new Ext.Action({
            text: this.translation.gettext('Edit Tag'),
            disabled: true,
            handler: this.handlers.editTag,
            iconCls: 'action_edit',
            scope: this
        });
        
        this.actions.deleteTag = new Ext.Action({
            text: this.translation.gettext('Delete Tag'),
            disabled: true,
            handler: this.handlers.deleteTag,
            iconCls: 'action_delete',
            scope: this
        });

    },
    
    displayTagsToolbar: function () {
        // if toolbar was allready created set active toolbar and return
        if (this.tagsToolbar) {
            Tine.Tinebase.MainScreen.setActiveToolbar(this.tagsToolbar, true);
            return;
        }
        
        var TagsQuickSearchField = new Ext.ux.SearchField({
            id: 'TagsQuickSearchField',
            width: 240,
            emptyText: Tine.Tinebase.translation._hidden('enter searchfilter')
        });
        
        TagsQuickSearchField.on('change', function () {
            Ext.getCmp('AdminTagsGrid').getStore().load({
                params: {
                    start: 0,
                    limit: 50
                }
            });
        }, this);
        
        this.tagsToolbar = new Ext.Toolbar({
            id: 'AdminTagsToolbar',
            split: false,
            //height: 26,
            items: [{
                xtype: 'buttongroup',
                columns: 5,
                items: [
                    Ext.apply(new Ext.Button(this.actions.addTag), {
                        scale: 'medium',
                        rowspan: 2,
                        iconAlign: 'top'
                    }), {xtype: 'tbspacer', width: 10},
                    Ext.apply(new Ext.Button(this.actions.editTag), {
                        scale: 'medium',
                        rowspan: 2,
                        iconAlign: 'top'
                    }), {xtype: 'tbspacer', width: 10},
                    Ext.apply(new Ext.Button(this.actions.deleteTag), {
                        scale: 'medium',
                        rowspan: 2,
                        iconAlign: 'top'
                    })
                ]
            }, '->', 
                this.translation.gettext('Search:'), 
                ' ',
                TagsQuickSearchField
            ]
        });

        Tine.Tinebase.MainScreen.setActiveToolbar(this.tagsToolbar, true);
    },

    displayTagsGrid: function () {
        // if grid panel was allready created set active content panel and return
        if (this.gridPanel) {
            Tine.Tinebase.MainScreen.setActiveContentPanel(this.gridPanel, true);
            return;
        }
        
        // the datastore
        var dataStore = new Ext.data.JsonStore({
            baseParams: {
                method: 'Admin.getTags'
            },
            root: 'results',
            totalProperty: 'totalcount',
            id: 'id',
            fields: Tine.Tinebase.Model.Tag,
            // turn on remote sorting
            remoteSort: true
        });
        
        dataStore.setDefaultSort('name', 'asc');

        dataStore.on('beforeload', function (dataStore, options) {
            options = options || {};
            options.params = options.params || {};
            options.params.query = Ext.getCmp('TagsQuickSearchField').getValue();
        }, this);
                
        // the paging toolbar
        var pagingToolbar = new Ext.PagingToolbar({
            pageSize: 50,
            store: dataStore,
            displayInfo: true,
            displayMsg: this.translation.gettext('Displaying tags {0} - {1} of {2}'),
            emptyMsg: this.translation.gettext("No tags to display")
        });
        
        // the columnmodel
        var columnModel = new Ext.grid.ColumnModel({
            defaults: {
                sortable: true,
                resizable: true
            },
            columns: [
                { id: 'color', header: this.translation.gettext('Color'), dataIndex: 'color', width: 25, renderer: function (color,meta,record) {
                    return '<div style="margin-top:1px;width: 8px; height: 8px; background-color:' + color + '; border-radius:5px;border: 1px solid black;" title="' + record.get('name') + ' (' +  _('Usage:&#160;') + record.get('occurrence') + ')">&#160;</div>';
                }},
                { id: 'name', header: this.translation.gettext('Name'), dataIndex: 'name', width: 200 },
                { id: 'description', header: this.translation.gettext('Description'), dataIndex: 'description', width: 500}
            ]
        });
        
        // the rowselection model
        var rowSelectionModel = new Ext.grid.RowSelectionModel({multiSelect: true});

        rowSelectionModel.on('selectionchange', function (selectionModel) {
            var rowCount = selectionModel.getCount();

            if (Tine.Tinebase.common.hasRight('manage', 'Admin', 'shared_tags')) {
                if (rowCount < 1) {
                    // no row selected
                    this.actions.deleteTag.setDisabled(true);
                    this.actions.editTag.setDisabled(true);
                } else if (rowCount > 1) {
                    // more than one row selected
                    this.actions.deleteTag.setDisabled(false);
                    this.actions.editTag.setDisabled(true);
                } else {
                    // only one row selected
                    this.actions.deleteTag.setDisabled(false);
                    this.actions.editTag.setDisabled(false);
                }
            }
        }, this);
        
        // the gridpanel
        this.gridPanel = new Ext.grid.GridPanel({
            id: 'AdminTagsGrid',
            store: dataStore,
            cm: columnModel,
            tbar: pagingToolbar,     
            autoSizeColumns: false,
            selModel: rowSelectionModel,
            enableColLock: false,
            autoExpandColumn: 'description',
            border: false,
            view: new Ext.grid.GridView({
                autoFill: true,
                forceFit: true,
                ignoreAdd: true,
                emptyText: this.translation.gettext('No tags to display')
            })            
        });
        
        this.gridPanel.on('rowcontextmenu', function (grid, rowIndex, eventObject) {
            eventObject.stopEvent();
            if (! grid.getSelectionModel().isSelected(rowIndex)) {
                grid.getSelectionModel().selectRow(rowIndex);
            }
            
            if (! this.contextMenu) {
                this.contextMenu = new Ext.menu.Menu({
                    id: 'ctxMenuTags', 
                    items: [
                        this.actions.editTag,
                        this.actions.deleteTag,
                        '-',
                        this.actions.addTag 
                    ]
                });
            }
            this.contextMenu.showAt(eventObject.getXY());
        }, this);
        
        this.gridPanel.on('rowdblclick', function (gridPar, rowIndexPar, ePar) {
            var record = gridPar.getStore().getAt(rowIndexPar);
            Tine.Admin.Tags.EditDialog.openWindow({
                tag: record,
                listeners: {
                    scope: this,
                    'update': function (record) {
                        this.reload();
                    }
                }
            });
        }, this);

        // add the grid to the layout
        Tine.Tinebase.MainScreen.setActiveContentPanel(this.gridPanel, true);
    },
    
    /**
     * update datastore with node values and load datastore
     */
    loadData: function () {
        var dataStore = Ext.getCmp('AdminTagsGrid').getStore();
        dataStore.load({ params: { start: 0, limit: 50 } });
    },

    show: function () {
        if (this.tagsToolbar === null || this.gridPanel) {
            this.initComponent();
        }

        this.displayTagsToolbar();
        this.displayTagsGrid();

        this.loadData();
    },
    
    reload: function () {
        if (Ext.ComponentMgr.all.containsKey('AdminTagsGrid')) {
            setTimeout("Ext.getCmp('AdminTagsGrid').getStore().reload()", 200);
        }
    }
};
/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philip Schuele <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2007-2010 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 * TODO         refactor this (don't use Ext.getCmp, etc.)
 */

/*global Ext, Tine, Locale*/

Ext.ns('Tine.Admin.Tags');

/**
 * @namespace   Tine.Admin.Tags
 * @class       Tine.Admin.Tags.EditDialog
 * @extends     Tine.widgets.dialog.EditRecord
 */
Tine.Admin.Tags.EditDialog = Ext.extend(Tine.widgets.dialog.EditRecord, {
    
    /**
     * var tag
     */
    tag: null,

    windowNamePrefix: 'AdminTagEditDialog_',
    id: 'tagDialog',
    layout: 'fit',
    border: false,
    labelWidth: 120,
    labelAlign: 'top',
    
    handlerApplyChanges: function (button, event, closeWindow) {
        var form = this.getForm();
        
        if (form.isValid()) {
            Ext.MessageBox.wait(this.translation.gettext('Please wait'), this.translation.gettext('Updating Tag'));
            
            var tag = this.tag;
            
            // fetch rights
            tag.data.rights = [];
            this.rightsStore.each(function (item) {
                tag.data.rights.push(item.data);
            });
            
            // fetch contexts
            tag.data.contexts = [];
            var anycontext = true;
            var contextPanel = Ext.getCmp('adminSharedTagsContextPanel');
            contextPanel.getRootNode().eachChild(function (node) {
                if (node.attributes.checked) {
                    tag.data.contexts.push(node.id);
                } else {
                    anycontext = false;
                }
            });
            if (anycontext) {
                tag.data.contexts = ['any'];
            }
            
            form.updateRecord(tag);
            
            Ext.Ajax.request({
                params: {
                    method: 'Admin.saveTag', 
                    tagData: tag.data
                },
                success: function (response) {
                    //if(this.window.opener.Tine.Admin.Tags) {
                    //    this.window.opener.Tine.Admin.Tags.Main.reload();
                    //}
                    this.fireEvent('update', Ext.util.JSON.encode(this.tag.data));
                    Ext.MessageBox.hide();
                    if (closeWindow === true) {
                        this.window.close();
                    } else {
                        this.onRecordLoad(response);
                    }
                },
                failure: function (result, request) {
                    Ext.MessageBox.alert(this.translation.gettext('Failed'), this.translation.gettext('Could not save tag.'));
                },
                scope: this 
            });
        } else {
            Ext.MessageBox.alert(this.translation.gettext('Errors'), this.translation.gettext('Please fix the errors noted.'));
        }
    },

    /**
     * function updateRecord
     */
    updateRecord: function (tagData) {
        // if tagData is empty (=array), set to empty object because array wont work!
        if (tagData.length === 0) {
            tagData = {};
        }
        this.tag = new Tine.Tinebase.Model.Tag(tagData, tagData.id ? tagData.id : 0);
        
        if (! tagData.rights) {
            tagData.rights = [{
                tag_id: '', //todo!
                account_name: 'Anyone',
                account_id: 0,
                account_type: 'anyone',
                view_right: true,
                use_right: true
            }];
        }
        
        this.rightsStore.loadData({
            results:    tagData.rights,
            totalcount: tagData.rights.length
        });
        
        this.anyContext = ! tagData.contexts || tagData.contexts.indexOf('any') > -1;
        this.createTreeNodes(tagData.appList);
        this.getForm().loadRecord(this.tag);
    },

    /**
     * function updateToolbarButtons
     */
    updateToolbarButtons: function (rights) {
       /* if(_rights.editGrant === true) {
            Ext.getCmp('tagDialog').action_saveAndClose.enable();
            Ext.getCmp('tagDialog').action_applyChanges.enable();
        }
       */

    },
    
    createTreeNodes: function (appList) {
        // clear old childs
        var toRemove = [];
        this.rootNode.eachChild(function (node) {
            toRemove.push(node);
        });
        
        for (var i = 0, j = appList.length; i < j; i += 1) {
            // don't duplicate tree nodes on 'apply changes'
            toRemove[i] ? toRemove[i].remove() : null;
            
            var appData = appList[i];
            if (appData.name === 'Tinebase') {
                continue;
            }
            // get translated app title
            var app = Tine.Tinebase.appMgr.get(appData.name),
                appTitle = (app) ? app.getTitle() : appData.name;
            
            this.rootNode.appendChild(new Ext.tree.TreeNode({
                text: appTitle,
                id: appData.id,
                checked: this.anyContext || this.tag.get('contexts').indexOf(appData.id) > -1,
                leaf: true,
                iconCls: 'x-tree-node-leaf-checkbox'
            }));
        }
    },
    /**
     * function display
     */
    getFormContents: function () {

        this.rootNode = new Ext.tree.TreeNode({
            text: this.translation.gettext('Allowed Contexts'),
            expanded: true,
            draggable: false,
            allowDrop: false
        });
        var contextPanel = new Ext.tree.TreePanel({
            title: this.translation.gettext('Context'),
            id: 'adminSharedTagsContextPanel',
            rootVisible: true,
            border: false,
            root: this.rootNode
        });
        // sort nodes in context panel by text property
        var treeSorter = new Ext.tree.TreeSorter(contextPanel, {
            folderSort: true,
            dir: "asc"
        });
        
        this.rightsPanel = new Tine.widgets.account.PickerGridPanel({
            title: this.translation.gettext('Account Rights'),
            store: this.rightsStore,
            recordClass: Tine.Admin.Model.TagRight,
            hasAccountPrefix: true,
            selectType: 'both',
            selectTypeDefault: 'group',
            showHidden: true,
            configColumns: [
                new Ext.ux.grid.CheckColumn({
                    header: this.translation.gettext('View'),
                    dataIndex: 'view_right',
                    width: 55
                }),
                new Ext.ux.grid.CheckColumn({
                    header: this.translation.gettext('Use'),
                    dataIndex: 'use_right',
                    width: 55
                })
            ]
        });

        var editTagDialog = {
            layout: 'border',
            border: false,
            items: [{
                region: 'north',
                xtype: 'columnform',
                border: false,
                autoHeight: true,
                items: [[{
                    columnWidth: 0.3,
                    fieldLabel: this.translation.gettext('Tag Name'), 
                    name: 'name',
                    allowBlank: false,
                    maxLength: 40
                }, {
                    columnWidth: 0.6,
                    name: 'description',
                    fieldLabel: this.translation.gettext('Description'),
                    anchor: '100%',
                    maxLength: 256
                }, {
                    xtype: 'colorfield',
                    columnWidth: 0.1,
                    fieldLabel: this.translation.gettext('Color'),
                    name: 'color'
                }]]
            }, {
                region: 'center',
                xtype: 'tabpanel',
                activeTab: 0,
                deferredRender: false,
                defaults: { autoScroll: true },
                border: true,
                plain: true,                    
                items: [
                    this.rightsPanel, 
                    contextPanel
                ]
            }]
        };
        
        return editTagDialog;
    },
    
    initComponent: function () {
        this.tag = this.tag ? this.tag : new Tine.Tinebase.Model.Tag({}, 0);
        
        this.translation = new Locale.Gettext();
        this.translation.textdomain('Admin');
        
        Ext.Ajax.request({
            scope: this,
            success: this.onRecordLoad,
            params: {
                method: 'Admin.getTag',
                tagId: this.tag.id
            }
        });
        
        this.rightsStore = new Ext.data.JsonStore({
            root: 'results',
            totalProperty: 'totalcount',
            id: 'account_id',
            fields: Tine.Admin.Model.TagRight
        });
        
        this.items = this.getFormContents();
        Tine.Admin.Tags.EditDialog.superclass.initComponent.call(this);
    },
    
    onRender: function (ct, position) {
        Tine.widgets.dialog.EditDialog.superclass.onRender.call(this, ct, position);
        
        // generalized keybord map for edit dlgs
        var map = new Ext.KeyMap(this.el, [{
            key: [10, 13], // ctrl + return
            ctrl: true,
            fn: this.handlerApplyChanges.createDelegate(this, [true], true),
            scope: this
        }]);

        this.loadMask = new Ext.LoadMask(ct, {msg: String.format(_('Transferring {0}...'), this.translation.gettext('Tag'))});
        this.loadMask.show();
    },
    
    onRecordLoad: function (response) {
        this.getForm().findField('name').focus(false, 250);
        var recordData = Ext.util.JSON.decode(response.responseText);
        this.updateRecord(recordData);
        
        if (! this.tag.id) {
            this.window.setTitle(this.translation.gettext('Add New Tag'));
        } else {
            this.window.setTitle(String.format(this.translation._('Edit Tag "{0}"'), this.tag.get('name')));
        }
        
        this.loadMask.hide();
    }     
});

/**
 * Admin Tag Edit Popup
 */
Tine.Admin.Tags.EditDialog.openWindow = function (config) {
    config.tag = config.tag ? config.tag : new Tine.Tinebase.Model.Tag({}, 0);
    var window = Tine.WindowFactory.getWindow({
        width: 650,
        height: 400,
        name: Tine.Admin.Tags.EditDialog.prototype.windowNamePrefix + config.tag.id,
        contentPanelConstructor: 'Tine.Admin.Tags.EditDialog',
        contentPanelConstructorConfig: config
    });
    return window;
};
/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philip Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 * TODO         refactor this (don't use Ext.getCmp, etc.)
 */
 
Ext.ns('Tine.Admin.Roles');

/*********************************** MAIN DIALOG ********************************************/

Tine.Admin.Roles.Main = {
    
    // references to crated toolbar and grid panel
    rolesToolbar: null,
    gridPanel: null,
    
    actions: {
        addRole: null,
        editRole: null,
        deleteRole: null
    },
    
    handlers: {
        /**
         * onclick handler for addBtn
         */
        addRole: function(_button, _event) {
            this.openEditWindow(null);
        },

        /**
         * onclick handler for editBtn
         */
        editRole: function(_button, _event) {
            var selectedRows = Ext.getCmp('AdminRolesGrid').getSelectionModel().getSelections();
            this.openEditWindow(selectedRows[0]);
        },

        /**
         * onclick handler for deleteBtn
         */
        deleteRole: function(_button, _event) {
            Ext.MessageBox.confirm(this.translation.gettext('Confirm'), this.translation.gettext('Do you really want to delete the selected roles?'), function(_button){
                if (_button == 'yes') {
                
                    var roleIds = new Array();
                    var selectedRows = Ext.getCmp('AdminRolesGrid').getSelectionModel().getSelections();
                    for (var i = 0; i < selectedRows.length; ++i) {
                        roleIds.push(selectedRows[i].id);
                    }
                    
                    roleIds = roleIds;
                    
                    Ext.Ajax.request({
                        url: 'index.php',
                        params: {
                            method: 'Admin.deleteRoles',
                            roleIds: roleIds
                        },
                        text: this.translation.gettext('Deleting role(s)...'),
                        success: function(_result, _request){
                            Ext.getCmp('AdminRolesGrid').getStore().reload();
                        },
                        failure: function(result, request){
                            Ext.MessageBox.alert(this.translation.gettext('Failed'), this.translation.gettext('Some error occurred while trying to delete the role.'));
                        }
                    });
                }
            }, this);
        }    
    },
    
    /**
     * open edit window
     * 
     * @param {} record
     */
    openEditWindow: function (record) {
        var popupWindow = Tine.Admin.Roles.EditDialog.openWindow({
            role: record,
            listeners: {
                scope: this,
                'update': function(record) {
                    this.reload();
                }
            }                
        });
    },
    
    /**
     * init roles grid
     */
    initComponent: function() {
        this.translation = new Locale.Gettext();
        this.translation.textdomain('Admin');
        
        this.actions.addRole = new Ext.Action({
            text: this.translation.gettext('Add Role'),
            disabled: true,
            handler: this.handlers.addRole,
            iconCls: 'action_permissions',
            scope: this
        });
        
        this.actions.editRole = new Ext.Action({
            text: this.translation.gettext('Edit Role'),
            disabled: true,
            handler: this.handlers.editRole,
            iconCls: 'action_edit',
            scope: this
        });
        
        this.actions.deleteRole = new Ext.Action({
            text: this.translation.gettext('Delete Role'),
            disabled: true,
            handler: this.handlers.deleteRole,
            iconCls: 'action_delete',
            scope: this
        });

    },
    
    displayRolesToolbar: function() {
        
        // if toolbar was allready created set active toolbar and return
        if (this.rolesToolbar)
        {
            Tine.Tinebase.MainScreen.setActiveToolbar(this.rolesToolbar, true);
            return;
        }
        
        var RolesQuickSearchField = new Ext.ux.SearchField({
            id: 'RolesQuickSearchField',
            width:240,
            emptyText: Tine.Tinebase.translation._hidden('enter searchfilter')
        });
        RolesQuickSearchField.on('change', function(){
            Ext.getCmp('AdminRolesGrid').getStore().load({
                params: {
                    start: 0,
                    limit: 50
                }
            });
        }, this);
        
        this.rolesToolbar = new Ext.Toolbar({
            id: 'AdminRolesToolbar',
            split: false,
            //height: 26,
            items: [{
                // create buttongroup to be consistent
                xtype: 'buttongroup',
                columns: 5, 
                items: [
                    Ext.apply(new Ext.Button(this.actions.addRole), {
                        scale: 'medium',
                        rowspan: 2,
                        iconAlign: 'top'
                    }), {xtype: 'tbspacer', width: 10},
                    Ext.apply(new Ext.Button(this.actions.editRole), {
                        scale: 'medium',
                        rowspan: 2,
                        iconAlign: 'top'
                    }), {xtype: 'tbspacer', width: 10},
                    Ext.apply(new Ext.Button(this.actions.deleteRole), {
                        scale: 'medium',
                        rowspan: 2,
                        iconAlign: 'top'
                    }), {xtype: 'tbspacer', width: 10}
                ]
            }, '->', 
                this.translation.gettext('Search:'), 
                ' ',
                RolesQuickSearchField
            ]
        });

        Tine.Tinebase.MainScreen.setActiveToolbar(this.rolesToolbar, true);
    },

    displayRolesGrid: function() {
        
        // if grid panel was allready created set active content panel and return
        if (this.gridPanel)
        {
            Tine.Tinebase.MainScreen.setActiveContentPanel(this.gridPanel, true);
            return;
        }
        
        if ( Tine.Tinebase.common.hasRight('manage', 'Admin', 'roles') ) {
            this.actions.addRole.setDisabled(false);
        }        
        
        // the datastore
        var dataStore = new Ext.data.DirectStore({
            api: {
                read: Tine.Admin.getRoles
            },
            
            reader: new Ext.data.JsonReader({
                root: 'results',
                idProperty: 'id',
                totalProperty: 'totalcount'
            }, Tine.Tinebase.Model.Role),
            
            remoteSort: true
        });
        
        dataStore.setDefaultSort('id', 'asc');

        dataStore.on('beforeload', function(_dataStore, _options) {
            _options = _options || {};
            _options.params = _options.params || {};
            _options.params.query = Ext.getCmp('RolesQuickSearchField').getValue();
        }, this);
        
        // the paging toolbar
        var pagingToolbar = new Ext.PagingToolbar({
            pageSize: 25,
            store: dataStore,
            displayInfo: true,
            displayMsg: this.translation.gettext('Displaying roles {0} - {1} of {2}'),
            emptyMsg: this.translation.gettext("No roles to display")
        });
        
        // the columnmodel
        var columnModel = new Ext.grid.ColumnModel({
            defaults: {
                sortable: true,
                resizable: true
            },
            columns: [
                { id: 'id', header: this.translation.gettext('ID'), dataIndex: 'id', hidden: true, width: 10 },
                { id: 'name', header: this.translation.gettext('Name'), dataIndex: 'name', width: 50 },
                { id: 'description', header: this.translation.gettext('Description'), dataIndex: 'description' }
            ]
        });
        
        // the rowselection model
        var rowSelectionModel = new Ext.grid.RowSelectionModel({multiSelect:true});

        rowSelectionModel.on('selectionchange', function(_selectionModel) {
            var rowCount = _selectionModel.getCount();

            if ( Tine.Tinebase.common.hasRight('manage', 'Admin', 'roles') ) {
                if(rowCount < 1) {
                    // no row selected
                    this.actions.deleteRole.setDisabled(true);
                    this.actions.editRole.setDisabled(true);
                } else if(rowCount > 1) {
                    // more than one row selected
                    this.actions.deleteRole.setDisabled(false);
                    this.actions.editRole.setDisabled(true);
                } else {
                    // only one row selected
                    this.actions.deleteRole.setDisabled(false);
                    this.actions.editRole.setDisabled(false);
                }
            }
        }, this);
        
        // the gridpanel
        this.gridPanel = new Ext.grid.GridPanel({
            id: 'AdminRolesGrid',
            store: dataStore,
            cm: columnModel,
            tbar: pagingToolbar,     
            autoSizeColumns: false,
            selModel: rowSelectionModel,
            enableColLock:false,
            autoExpandColumn: 'n_family',
            border: false,
            view: new Ext.grid.GridView({
                autoFill: true,
                forceFit:true,
                ignoreAdd: true,
                emptyText: this.translation.gettext('No roles to display')
            })            
        });
        
        this.gridPanel.on('rowcontextmenu', function(_grid, _rowIndex, _eventObject) {
            _eventObject.stopEvent();
            if(!_grid.getSelectionModel().isSelected(_rowIndex)) {
                _grid.getSelectionModel().selectRow(_rowIndex);
            }
            
            if (! this.contextMenu) {
                this.contextMenu = new Ext.menu.Menu({
                    id: 'ctxMenuRoles', 
                    items: [
                        this.actions.editRole,
                        this.actions.deleteRole,
                        '-',
                        this.actions.addRole 
                    ]
                });
            }
            this.contextMenu.showAt(_eventObject.getXY());
        }, this);
        
        this.gridPanel.on('rowdblclick', function(_gridPar, _rowIndexPar, ePar) {
            if ( Tine.Tinebase.common.hasRight('manage', 'Admin', 'roles') ) {
                var record = _gridPar.getStore().getAt(_rowIndexPar);
                this.openEditWindow(record);
            }
        }, this);

        // add the grid to the layout
        Tine.Tinebase.MainScreen.setActiveContentPanel(this.gridPanel, true);
    },
    
    /**
     * update datastore with node values and load datastore
     */
    loadData: function() 
    {
        var dataStore = Ext.getCmp('AdminRolesGrid').getStore();
        dataStore.load({ params: { start:0, limit:50 } });
    },

    show: function() 
    {
        if (this.rolesToolbar === null || this.gridPanel === null) {
            this.initComponent();
        }

        this.displayRolesToolbar();
        this.displayRolesGrid();

        this.loadData();
    },
    
    reload: function() {
        if(Ext.ComponentMgr.all.containsKey('AdminRolesGrid')) {
            setTimeout ("Ext.getCmp('AdminRolesGrid').getStore().reload()", 200);
        }
    }
};


/**
 * Model of a right
 */
Tine.Admin.Roles.Right = Ext.data.Record.create([
    {name: 'application_id'},
    {name: 'right'}
]);


/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philip Schuele <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2007-2010 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 * TODO         refactor this (don't use Ext.getCmp, etc.)
 */

/*global Ext, Tine, Locale*/

Ext.ns('Tine.Admin.Roles');

/**
 * @namespace   Tine.Admin.Roles
 * @class       Tine.Admin.Roles.EditDialog
 * @extends     Tine.widgets.dialog.EditRecord
 */
Tine.Admin.Roles.EditDialog = Ext.extend(Tine.widgets.dialog.EditRecord, {

    /**
     * @cfg {Tine.Tinebase.Model.Role}
     */
    role: null,
    
    /**
     * @property {Object} holds allRights (possible rights)
     */
    allRights: null,
    
    /**
     * @property {Ext.tree.treePanel}
     */
    rightsTreePanel: null,
    
    windowNamePrefix: 'rolesEditWindow_',
    
    layout: 'fit',
    border: false,
    id: 'roleDialog',
    labelWidth: 120,
    labelAlign: 'top',
    
    /**
     * check if right is set for application and get the record id
     * @private
     */
    getRightId: function (applicationId, right) {
        var id = false,
            result = null;
        
        this.rightsDataStore.each(function (item) {
            if (item.data.application_id === applicationId && item.data.right === right) {
                result = item.id;
                return;
            }
        });
        
        return result;
    },  
    
    handlerApplyChanges: function (button, event, closeWindow) {
        var form = this.getForm();
        
        if (form.isValid()) {
            // get role members
            var roleGrid = Ext.getCmp('roleMembersGrid');

            Ext.MessageBox.wait(this.translation.gettext('Please wait'), this.translation.gettext('Updating Memberships'));
            
            var roleMembers = [];
            this.membersStore.each(function (record) {
                roleMembers.push(record.data);
            });

            // get role rights                
            var roleRights = [],
                rightsStore = this.rightsDataStore;
            
            rightsStore.each(function (record) {
                roleRights.push(record.data);
            });

            // update form               
            form.updateRecord(this.role);

            /*********** save role members & form ************/
            
            Ext.Ajax.request({
                params: {
                    method: 'Admin.saveRole', 
                    roleData: this.role.data,
                    roleMembers: roleMembers,
                    roleRights: roleRights
                },
                timeout: 300000, // 5 minutes
                success: function (response) {
                    this.fireEvent('update', Ext.util.JSON.encode(this.role.data));
                    Ext.MessageBox.hide();
                    if (closeWindow === true) {
                        this.window.close();
                    } else {
                        this.onRecordLoad(response);
                    }
                },
                failure: function (result, request) {
                    Ext.MessageBox.alert(this.translation.gettext('Failed'), this.translation.gettext('Could not save role.'));
                },
                scope: this 
            });
                
            
        } else {
            Ext.MessageBox.alert(this.translation.gettext('Errors'), this.translation.gettext('Please fix the errors noted.'));
        }
    },
    
    handlerDelete: function (button, event) {
        var roleIds = [this.role.id];
            
        Ext.Ajax.request({
            url: 'index.php',
            params: {
                method: 'Admin.deleteRoles', 
                roleIds: roleIds
            },
            text: this.translation.gettext('Deleting role...'),
            success: function (result, request) {
                if (window.opener.Tine.Admin.Roles) {
                    window.opener.Tine.Admin.Roles.Main.reload();
                }
                window.close();
            },
            failure: function (result, request) {
                Ext.MessageBox.alert(this.translation.gettext('Failed'), this.translation.gettext('Some error occurred while trying to delete the role.'));
            } 
        });
    },

    updateRecord: function (roleData) {
        // if roleData is empty (=array), set to empty object because array won't work!
        if (roleData.length === 0) {
            roleData = {};
        }
        this.role = new Tine.Tinebase.Model.Role(roleData, roleData.id ? roleData.id : 0);
        
        this.membersStore.loadData(this.role.get('roleMembers'));
        this.rightsDataStore.loadData(this.role.get('roleRights'));
        this.allRights = this.role.get('allRights');
        this.createRightsTreeNodes();
    },

    /**
     * creates the rights tree
     *
     */
    initRightsTree: function () {
        this.rightsTreePanel = new Ext.tree.TreePanel({
            title: this.translation.gettext('Rights'),
            autoScroll: true,
            rootVisible: false,
            border: false
        });
        
        // sort nodes by text property
        this.treeSorter = new Ext.tree.TreeSorter(this.rightsTreePanel, {
            folderSort: true,
            dir: "asc"
        });
        
        // set the root node
        var treeRoot = new Ext.tree.TreeNode({
            text: 'root',
            draggable: false,
            allowDrop: false,
            id: 'root'
        });

        this.rightsTreePanel.setRootNode(treeRoot);
    },
    
    /**
     * create nodes for rights tree (apps + app rights)
     * 
     * @return {Ext.tree.TreePanel}
     */
    createRightsTreeNodes: function () {
        var treeRoot = this.rightsTreePanel.getRootNode();
        
        var toRemove = [];
        treeRoot.eachChild(function (node) {
            toRemove.push(node);
        });
        
        var expandNode = (this.allRights.length > 5) ? false : true;
        
        // add nodes to tree        
        for (var i = 0; i < this.allRights.length; i += 1) {
            // don't duplicate tree nodes on 'apply changes'
            var remove = toRemove[i] ? toRemove[i].remove() : null;
            this.allRights[i].text = this.translateAppTitle(this.allRights[i].text);
            var node = new Ext.tree.TreeNode(this.allRights[i]);
            node.attributes.application_id = this.allRights[i].application_id;
            node.expanded = expandNode;
            treeRoot.appendChild(node);
            
            // append children          
            if (this.allRights[i].children) {
                for (var j = 0; j < this.allRights[i].children.length; j += 1) {
                    var childData = this.allRights[i].children[j];
                    childData.leaf = true;
                    
                    // check if right is set
                    childData.checked = !!this.getRightId(this.allRights[i].application_id, childData.right);
                    childData.iconCls = "x-tree-node-leaf-checkbox";
                    var child = new Ext.tree.TreeNode(childData);
                    child.attributes.right = childData.right;
                    
                    child.on('checkchange', function (node, checked) {
                        var applicationId = node.parentNode.attributes.application_id;
                    
                        // put it in the storage or remove it                        
                        if (checked) {
                            this.rightsDataStore.add(
                                new Ext.data.Record({
                                    right: node.attributes.right,
                                    application_id: applicationId
                                })
                            );
                        } else {
                            var rightId = this.getRightId(applicationId, node.attributes.right);
                            this.rightsDataStore.remove(this.rightsDataStore.getById(rightId));
                        }   
                    }, this);
                    
                    node.appendChild(child);
                }       
            }
        }     
        
        return this.rightsTreePanel;
    },
    
    /**
     * translate and return app title
     * 
     * TODO try to generalize this fn as this gets used in Tags.js + Applications.js as well 
     *      -> this could be moved to Tine.Admin.Application after Admin js refactoring
     */
    translateAppTitle: function (appName) {
        var app = Tine.Tinebase.appMgr.get(appName);
        return (app) ? app.getTitle() : appName;
    },
    
    /**
     * returns form
     * 
     */
    getFormContents: function () {
        this.accountPickerGridPanel = new Tine.widgets.account.PickerGridPanel({
            title: this.translation.gettext('Members'),
            store: this.membersStore,
            anchor: '100% 100%',
            groupRecordClass: Tine.Admin.Model.Group,
            selectType: 'both',
            selectAnyone: false,
            selectTypeDefault: 'group',
            showHidden: true
        });
        
        this.initRightsTree();
        
        /******* THE edit dialog ********/
        
        var editRoleDialog = {
            layout: 'border',
            border: false,
            items: [{
                region: 'north',
                layout: 'form',
                border: false,
                autoHeight: true,
                items: [{
                    xtype: 'textfield',
                    fieldLabel: this.translation.gettext('Role Name'), 
                    name: 'name',
                    anchor: '100%',
                    allowBlank: false,
                    maxLength: 128
                }, {
                    xtype: 'textarea',
                    name: 'description',
                    fieldLabel: this.translation.gettext('Description'),
                    grow: false,
                    preventScrollbars: false,
                    anchor: '100%',
                    height: 60
                }]
            }, {
                xtype: 'tabpanel',
                plain: true,
                region: 'center',
                activeTab: 0,
                items: [
                    this.accountPickerGridPanel,
                    this.rightsTreePanel
                ]
            }]
        };
        
        return editRoleDialog;
    },
    
    initComponent: function () {
        this.role = this.role ? this.role : new Tine.Tinebase.Model.Role({}, 0);
        
        this.translation = new Locale.Gettext();
        this.translation.textdomain('Admin');
        
        Ext.Ajax.request({
            scope: this,
            success: this.onRecordLoad,
            params: {
                method: 'Admin.getRole',
                roleId: this.role.id
            }
        });
        
        // init role members store
        this.membersStore = new Ext.data.JsonStore({
            root: 'results',
            totalProperty: 'totalcount',
            id: 'id',
            fields: Tine.Tinebase.Model.Account
        });
        
        // init rights store
        this.rightsDataStore = new Ext.data.JsonStore({
            root: 'results',
            totalProperty: 'totalcount',
            fields: Tine.Admin.Roles.Right
        });
        
        this.items = this.getFormContents();
        
        Tine.Admin.Groups.EditDialog.superclass.initComponent.call(this);
    },
    
    onRender: function (ct, position) {
        Tine.widgets.dialog.EditDialog.superclass.onRender.call(this, ct, position);
        
        // generalized keybord map for edit dlgs
        var map = new Ext.KeyMap(this.el, [{
            key: [10, 13], // ctrl + return
            ctrl: true,
            fn: this.handlerApplyChanges.createDelegate(this, [true], true),
            scope: this
        }]);

        this.loadMask = new Ext.LoadMask(ct, {msg: String.format(_('Transferring {0}...'), this.translation.gettext('Role'))});
        this.loadMask.show();
    },
    
    onRecordLoad: function (response) {
        this.getForm().findField('name').focus(false, 250);
        var recordData = Ext.util.JSON.decode(response.responseText);
        this.updateRecord(recordData);
        
        if (! this.role.id) {
            this.window.setTitle(this.translation.gettext('Add New Role'));
        } else {
            this.window.setTitle(String.format(this.translation.gettext('Edit Role "{0}"'), this.role.get('name')));
        }
        
        this.getForm().loadRecord(this.role);
        
        this.loadMask.hide();
    }
    
});

/**
 * Roles Edit Popup
 */
Tine.Admin.Roles.EditDialog.openWindow = function (config) {
    config.role = config.role ? config.role : new Tine.Tinebase.Model.Role({}, 0);
    var window = Tine.WindowFactory.getWindow({
        width: 400,
        height: 600,
        name: Tine.Admin.Roles.EditDialog.prototype.windowNamePrefix + config.role.id,
        contentPanelConstructor: 'Tine.Admin.Roles.EditDialog',
        contentPanelConstructorConfig: config
    });
    return window;
};
/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2011 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */

Ext.ns('Tine.Admin');

/**
 * @namespace   Tine.widgets.tags
 * @class       Tine.Admin.ApplicationFilter
 * @extends     Tine.widgets.grid.FilterModel
 */
Tine.Admin.ApplicationFilter = Ext.extend(Tine.widgets.grid.FilterModel, {
    /**
     * @cfg {Tine.Tinebase.Application} app
     */
    app: null,
    
    field: 'application_id',
    defaultOperator: 'equals',
    
    /**
     * @private
     */
    initComponent: function() {
        Tine.Admin.ApplicationFilter.superclass.initComponent.call(this);
        
        this.operators = ['equals', 'not'];
        this.label = this.app.i18n._('Application');
    },
    
    /**
     * value renderer
     * 
     * @param {Ext.data.Record} filter line
     * @param {Ext.Element} element to render to 
     */
    valueRenderer: function(filter, el) {
        this.appStore = new Ext.data.JsonStore({
            root: 'results',
            totalProperty: 'totalcount',
            fields: Tine.Admin.Model.Application
        });
        this.appStore.loadData({
            results:    Tine.Tinebase.registry.get('userApplications'),
            totalcount: Tine.Tinebase.registry.get('userApplications').length
        });
        
        var value = new Ext.form.ComboBox({
            app: this.app,
            name: 'application_id',
            store: this.appStore,
            displayField: 'name',
            valueField: 'id',
            filter: filter,
            width: 200,
            id: 'tw-ftb-frow-valuefield-' + filter.id,
            value: filter.data.value ? filter.data.value : this.defaultValue,
            mode: 'local',
            renderTo: el
        });
        value.on('specialkey', function(field, e){
             if(e.getKey() == e.ENTER){
                 this.onFiltertrigger();
             }
        }, this);
        
        return value;
    }
});
Tine.widgets.grid.FilterToolbar.FILTERS['admin.application'] = Tine.Admin.ApplicationFilter;

/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2011 Metaways Infosystems GmbH (http://www.metaways.de)
 */

/*global Ext, Tine*/ 

Ext.ns('Tine.Admin.container');

/**
 * Containers 'mainScreen'
 * 
 * @static
 */
Tine.Admin.container.show = function () {
    var app = Tine.Tinebase.appMgr.get('Admin');
    if (! Tine.Admin.container.gridPanel) {
        Tine.Admin.container.gridPanel = new Tine.Admin.container.GridPanel({
            app: app
        });
    }
    else {
        Tine.Admin.container.gridPanel.loadGridData.defer(100, Tine.Admin.container.gridPanel, []);
    }
    
    Tine.Tinebase.MainScreen.setActiveContentPanel(Tine.Admin.container.gridPanel, true);
    Tine.Tinebase.MainScreen.setActiveToolbar(Tine.Admin.container.gridPanel.actionToolbar, true);
};

/************** models *****************/
Ext.ns('Tine.Admin.Model');

/**
 * Model of a container
 */
Tine.Admin.Model.Container = Tine.Tinebase.data.Record.create(Tine.Tinebase.Model.Container.getFieldDefinitions().concat([
    {name: 'note'}
]), {
    appName: 'Admin',
    modelName: 'Container',
    idProperty: 'id',
    titleProperty: 'name',
    // ngettext('Container', 'Containers', n);
    recordName: 'Container',
    recordsName: 'Containers'
});

/**
 * returns default account data
 * 
 * @namespace Tine.Admin.Model.Container
 * @static
 * @return {Object} default data
 */
Tine.Admin.Model.Container.getDefaultData = function () {
    return {
        type: 'shared',
        backend: 'Sql'
    };
};

/************** backend *****************/

Tine.Admin.containerBackend = new Tine.Tinebase.data.RecordProxy({
    appName: 'Admin',
    modelName: 'Container',
    recordClass: Tine.Admin.Model.Container,
    idProperty: 'id'
});
/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2011 Metaways Infosystems GmbH (http://www.metaways.de)
 */
 
/*global Ext, Tine*/

Ext.ns('Tine.Admin.container');

/**
 * @namespace   Tine.Admin.container
 * @class       Tine.Admin.ContainerEditDialog
 * @extends     Tine.widgets.dialog.EditDialog
 * 
 * <p>Container Edit Dialog</p>
 * <p>
 * </p>
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Admin.ContainerEditDialog
 * 
 * TODO add note for personal containers (note is sent to container owner)
 */
Tine.Admin.ContainerEditDialog = Ext.extend(Tine.widgets.dialog.EditDialog, {
    
    /**
     * @private
     */
    windowNamePrefix: 'containerEditWindow_',
    appName: 'Admin',
    recordClass: Tine.Admin.Model.Container,
    recordProxy: Tine.Admin.containerBackend,
    evalGrants: false,
    
    /**
     * executed after record got updated from proxy
     */
    onRecordLoad: function () {
        Tine.Admin.ContainerEditDialog.superclass.onRecordLoad.apply(this, arguments);
        
        // load grants store if editing record
        if (this.record && this.record.id) {
            this.grantsStore.loadData({
                results:    this.record.get('account_grants'),
                totalcount: this.record.get('account_grants').length
            });
        }
    },    
    
    /**
     * executed when record gets updated from form
     */
    onRecordUpdate: function () {
        Tine.Admin.ContainerEditDialog.superclass.onRecordUpdate.apply(this, arguments);
        
        // get grants from grants grid
        this.record.set('account_grants', '');
        var grants = [];
        this.grantsStore.each(function(grant){
            grants.push(grant.data);
        });
        this.record.set('account_grants', grants);
    },
    
    /**
     * create grants store + grid
     * 
     * @return {Tine.widgets.container.GrantsGrid}
     */
    initGrantsGrid: function () {
        this.grantsStore = new Ext.data.JsonStore({
            root: 'results',
            totalProperty: 'totalcount',
            id: 'account_id',
            fields: Tine.Tinebase.Model.Grant
        });
       
        this.grantsGrid = new Tine.widgets.container.GrantsGrid({
            flex: 1,
            store: this.grantsStore,
            grantContainer: this.record.data,
            alwaysShowAdminGrant: true,
            showHidden: true
        });
        
        return this.grantsGrid;
    },
    
    /**
     * returns dialog
     */
    getFormItems: function () {
        var userApplications = Tine.Tinebase.registry.get('userApplications');
        this.appStore = new Ext.data.JsonStore({
            root: 'results',
            totalProperty: 'totalcount',
            fields: Tine.Admin.Model.Application
        });
        this.appStore.loadData({
            results: userApplications,
            totalcount: userApplications.length
        });
        
        return {
            layout: 'vbox',
            layoutConfig: {
                align: 'stretch',
                pack: 'start'
            },
            border: false,
            items: [{
                xtype: 'columnform',
                border: false,
                autoHeight: true,
                items: [[{
                    columnWidth: 0.225,
                    fieldLabel: this.app.i18n._('Name'), 
                    name: 'name',
                    allowBlank: false,
                    maxLength: 40
                }, {
                    xtype: 'combo',
                    readOnly: this.record.id != 0,
                    store: this.appStore,
                    columnWidth: 0.225,
                    name: 'application_id',
                    displayField: 'name',
                    valueField: 'id',
                    fieldLabel: this.app.i18n._('Application'),
                    mode: 'local',
                    anchor: '100%',
                    allowBlank: false,
                    forceSelection: true
                }, 
                    // TODO: in master we can fill a combo using app.registry.get('models'), 
                    //        but at the moment this is just a ro textfield, and we have no 
                    //        apps with more than one model having containers
                {
                    readOnly: 1,
                    columnWidth: 0.225,
                    name: 'model',
                    fieldLabel: this.app.i18n._('Model'),
                    anchor: '100%',
                    allowBlank: true
                }, {
                    xtype: 'combo',
                    columnWidth: 0.225,
                    name: 'type',
                    fieldLabel: this.app.i18n._('Type'),
                    store: [['personal', this.app.i18n._('personal')], ['shared', this.app.i18n._('shared')]],
                    listeners: {
                        scope: this,
                        select: function (combo, record) {
                            this.getForm().findField('note').setDisabled(record.data.field1 === 'shared');
                        }
                    },
                    mode: 'local',
                    anchor: '100%',
                    allowBlank: false,
                    forceSelection: true
                }, {
                    xtype: 'colorfield',
                    columnWidth: 0.1,
                    fieldLabel: this.app.i18n._('Color'),
                    name: 'color'
                }]]
            }, 
                this.initGrantsGrid(), {
                    emptyText: this.app.i18n._('Note for Owner'),
                    disabled: this.record.get('type') == 'shared',
                    xtype: 'textarea',
                    border: false,
                    autoHeight: true,
                    name: 'note'
                }
               ]
        };
    }
});

/**
 * Container Edit Popup
 * 
 * @param   {Object} config
 * @return  {Ext.ux.Window}
 */
Tine.Admin.ContainerEditDialog.openWindow = function (config) {
    var window = Tine.WindowFactory.getWindow({
        width: 600,
        height: 400,
        name: Tine.Admin.ContainerEditDialog.prototype.windowNamePrefix + Ext.id(),
        contentPanelConstructor: 'Tine.Admin.ContainerEditDialog',
        contentPanelConstructorConfig: config
    });
    return window;
};
/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2011 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
Ext.ns('Tine.Admin.container');


/**
 * Container grid panel
 * 
 * @namespace   Tine.Admin.container
 * @class       Tine.Admin.container.GridPanel
 * @extends     Tine.widgets.grid.GridPanel
 */
Tine.Admin.container.GridPanel = Ext.extend(Tine.widgets.grid.GridPanel, {
    
    /**
     * @cfg
     */
    newRecordIcon: 'admin-action-add-container',
    recordClass: Tine.Admin.Model.Container,
    recordProxy: Tine.Admin.containerBackend,
    defaultSortInfo: {field: 'name', direction: 'ASC'},
    evalGrants: false,
    gridConfig: {
        autoExpandColumn: 'name'
    },
    
    /**
     * initComponent
     */
    initComponent: function() {
        this.gridConfig.cm = this.getColumnModel();
        this.initFilterToolbar();
        
        this.plugins = this.plugins || [];
        this.plugins.push(this.filterToolbar);
        
        Tine.Admin.container.GridPanel.superclass.initComponent.call(this);
    },
    
    /**
     * returns column model
     * 
     * @return Ext.grid.ColumnModel
     * @private
     */
    getColumnModel: function() {
        return new Ext.grid.ColumnModel({
            defaults: {
                sortable: true,
                hidden: true,
                resizable: true
            },
            columns: this.getColumns()
        });
    },
    
    /**
     * returns columns
     * @private
     * @return Array
     */
    getColumns: function(){
        return [
            { header: this.app.i18n._('ID'), id: 'id', dataIndex: 'id', width: 50},
            { header: this.app.i18n._('Container Name'), id: 'name', dataIndex: 'name', hidden: false, width: 200},
            { header: this.app.i18n._('Application'), id: 'application_id', dataIndex: 'application_id', hidden: false, width: 100, renderer: this.appRenderer, scope: this},
            { header: this.app.i18n._('Type'), id: 'type', dataIndex: 'type', hidden: false, width: 80, renderer: this.typeRenderer, scope: this}
        ];
    },
    
    /**
     * returns application name
     * 
     * @param {Object} value
     * @return {String}
     */
    appRenderer: function(value) {
        return this.app.i18n._(value.name);
    },
    
    /**
     * returns translated type
     * 
     * @param {Object} value
     * @return {String}
     */
    typeRenderer: function(value) {
        return this.app.i18n._(value);
    },
    
    /**
     * initialises filter toolbar
     */
    initFilterToolbar: function() {
        this.filterToolbar = new Tine.widgets.grid.FilterToolbar({
            filterModels: [
                {label: this.app.i18n._('Container'),       field: 'query',    operators: ['contains']},
                {label: this.app.i18n._('Type'),            field: 'type',     operators: ['contains']},
                {filtertype: 'admin.application', app: this.app}
            ],
            defaultFilter: 'query',
            filters: [
                {field: 'type', operator: 'equals', value: 'shared'}
            ],
            plugins: [
                new Tine.widgets.grid.FilterToolbarQuickFilterPlugin()
            ]
        });
    }    
});
/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2011 Metaways Infosystems GmbH (http://www.metaways.de)
 */

/*global Ext, Tine*/ 

Ext.ns('Tine.Admin.customfield');

/**
 * Customfields 'mainScreen'
 * 
 * @static
 */
Tine.Admin.customfield.show = function () {
    var app = Tine.Tinebase.appMgr.get('Admin');
    if (! Tine.Admin.customfield.gridPanel) {
        Tine.Admin.customfield.gridPanel = new Tine.Admin.customfield.GridPanel({
            app: app
        });
    }
    else {
        Tine.Admin.customfield.gridPanel.loadGridData.defer(100, Tine.Admin.customfield.gridPanel, []);
    }
    
    Tine.Tinebase.MainScreen.setActiveContentPanel(Tine.Admin.customfield.gridPanel, true);
    Tine.Tinebase.MainScreen.setActiveToolbar(Tine.Admin.customfield.gridPanel.actionToolbar, true);
};

/************** models *****************/
Ext.ns('Tine.Admin.Model');

/**
 * Model of a customfield
 */
Tine.Admin.Model.Customfield = Tine.Tinebase.data.Record.create(Tine.Tinebase.Model.Customfield.prototype.fields.items, {
    appName: 'Admin',
    modelName: 'Customfield',
    idProperty: 'id',
    titleProperty: 'name',
    // ngettext('Customfield', 'Customfields', n);
    recordName: 'Customfield',
    recordsName: 'Customfields'
});

/************** backend *****************/

Tine.Admin.customfieldBackend = new Tine.Tinebase.data.RecordProxy({
    appName: 'Admin',
    modelName: 'Customfield',
    recordClass: Tine.Admin.Model.Customfield,
    idProperty: 'id'
});
/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2011-2012 Metaways Infosystems GmbH (http://www.metaways.de)
 */
 
/*global Ext, Tine*/

Ext.ns('Tine.Admin.customfield');

/**
 * @namespace   Tine.Admin.customfield
 * @class       Tine.Admin.CustomfieldEditDialog
 * @extends     Tine.widgets.dialog.EditDialog
 * 
 * <p>Customfield Edit Dialog</p>
 * <p>
 * </p>
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Admin.CustomfieldEditDialog
 * 
 */
Tine.Admin.CustomfieldEditDialog = Ext.extend(Tine.widgets.dialog.EditDialog, {
    
    /**
     * @private
     */
    windowNamePrefix: 'customfieldEditWindow_',
    appName: 'Admin',
    recordClass: Tine.Admin.Model.Customfield,
    recordProxy: Tine.Admin.customfieldBackend,
    evalGrants: false,
    
    /**
     * definition properties for cusomfield
     * @type {Array}
     */
    definitionFields: ['label', 'type', 'value_search', 'length', 'required'],
    
    /**
     * ui properties for customfield
     * @type {Array}
     */
    uiconfigFields: ['order', 'group'],
    
    /**
     * type of field with stores
     * @type {Array}
     */
    fieldsWithStore: ['record', 'keyField'],
    
    /**
     * currently selected field type
     * @type 
     */
    fieldType: null,
        
    /**
     * executed after record got updated from proxy
     *  - load values for customfield definition and ui
     */
    onRecordLoad: function () {
        Tine.Admin.CustomfieldEditDialog.superclass.onRecordLoad.apply(this, arguments);
        
        // get definition
        if (this.rendered && this.record.get('definition')) {
            this.fieldType = this.record.get('definition').type;
            
            // load definition values
            Ext.each(this.definitionFields, function (name) {
                this.getForm().findField(name).setValue(this.record.get('definition')[name]);
            }, this);
            
            // load ui config values
            Ext.each(this.uiconfigFields, function (name) {
                this.getForm().findField(name).setValue(this.record.get('definition').uiconfig[name]);
            }, this);
            
            // load specific values for fields with store
            if (this.record.get('definition')[this.fieldType + 'Config']) {
                this[this.fieldType + 'Config'] = this.record.get('definition')[this.fieldType + 'Config'];
            }
        }
    },    
    
    /**
     * executed when record gets updated from form
     *  - get values for customfield definition and ui
     */
    onRecordUpdate: function () {
        Tine.Admin.CustomfieldEditDialog.superclass.onRecordUpdate.apply(this, arguments);
        
        // field definition
        this.record.data.definition = {
            uiconfig: {}
        };
        
        // save definition
        Ext.each(this.definitionFields, function (name) {
            var field = this.getForm().findField(name);
            
            if (! field.disabled && (name !== 'value_search' || (name === 'value_search' && field.getValue() == 1))) {
                this.record.data.definition[name] = field.getValue();
            }
        }, this);
        
        // save ui config
        Ext.each(this.uiconfigFields, function (name) {
            this.record.data.definition.uiconfig[name] = this.getForm().findField(name).getValue();;
        }, this);
        
        // set specific values for fields with stores
        Ext.each(this.fieldsWithStore, function (field) {
            if (this.fieldsWithStore.indexOf(this.fieldType) !== -1 && this[field + 'Config']) {
                this.record.data.definition[field + 'Config'] = this[field + 'Config'];
            }
        }, this);
    },
    
    /**
     * Apply changes handler
     *  - validate some additional data before saving
     * 
     * @param {Ext.Button} button
     * @param {Ext.EventObject} event
     * @param {Boolean} closeWindow
     */
    onApplyChanges: function () {
        if (this.fieldsWithStore.indexOf(this.fieldType) !== -1 && ! this[this.fieldType + 'Config']) {
            Ext.Msg.alert(_('Errors'), this.app.i18n._('Please configure store for this field type'));
            return;
        }
        
        Tine.Admin.CustomfieldEditDialog.superclass.onApplyChanges.apply(this, arguments);
    },
    
    /**
     * Called on type combo select
     * 
     * @param {Ext.form.Combobox}   combo
     * @param {Ext.data.Recod}      record
     * @param {Int}                 index
     */
    onTypeComboSelect: function (combo, record, index) {
        this.fieldType = combo.getValue();
        
        this.getForm().findField('length').setDisabled(this.fieldType !== 'string');
        this.getForm().findField('value_search').setValue(this.fieldType === 'searchcombo' ? 1 : 0);
        this.configureStoreBtn.setDisabled(this.fieldsWithStore.indexOf(this.fieldType) === -1);
    },
    
    /**
     * Set field with store config
     */ 
    onStoreWindowOK: function () {
        if (this[this.fieldType + 'Store'].isValid()) {
            this[this.fieldType + 'Config'] = {
                value: {
                    records: this[this.fieldType + 'Store'].getValue()      
                }
            };
            
            // set default if defined for keyField
            if (this.fieldType === 'keyField') {
                var defaultRecIdx = this[this.fieldType + 'Store'].store.findExact('default', true);
                if (defaultRecIdx !== -1) {
                    this[this.fieldType + 'Config'].value['default'] = this[this.fieldType + 'Store'].store.getAt(defaultRecIdx).get('id');
                }
            }
             
            this.onStoreWindowClose();
        }
    },
    
    /**
     * Close store Window
     */
    onStoreWindowClose: function () {
        this.storeWindow.purgeListeners();
        this.storeWindow.close();
    },
    
    /**
     * Get available model for given application
     * 
     *  @param {Mixed} application
     *  @param {Boolean} customFieldModel
     */
    getApplicationModels: function (application, customFieldModel) {
        var models      = [],
            useModel,
            appName     = Ext.isString(application) ? application : application.get('name'),
            app         = Tine.Tinebase.appMgr.get(appName),
            trans       = app && app.i18n ? app.i18n : Tine.Tinebase.translation,
            appModels   = Tine[appName].Model;
        
        if (appModels) {
            for (var model in appModels) {
                if (appModels.hasOwnProperty(model) && typeof appModels[model].getMeta === 'function') {
                    if (customFieldModel && appModels[model].getField('customfields')) {
                        useModel = appModels[model].getMeta('appName') + '_Model_' + appModels[model].getMeta('modelName');
                        
                        Tine.log.info('Found model with customfields property: ' + useModel);
                        models.push([useModel, trans.n_(appModels[model].getMeta('recordName'), appModels[model].getMeta('recordsName'), 1)]);
                    }
                    else if (! customFieldModel) {
                        useModel = 'Tine.' + appModels[model].getMeta('appName') + '.Model.' + appModels[model].getMeta('modelName');
                        
                        Tine.log.info('Found model: ' + useModel);
                        models.push([useModel, trans.n_(appModels[model].getMeta('recordName'), appModels[model].getMeta('recordsName'), 1)]);
                    }
                }
            }
        }
        
        return models;
    },
    
    /**
     * Create store for keyField
     * 
     * @returns {Tine.widgets.grid.QuickaddGridPanel}
     */
    initKeyFieldStore: function () {
        Tine.log.info('Initialize keyField store config');
        
        var self = this,
            
            storeEntry = Ext.data.Record.create([
                {name: 'default'},
                {name: 'id'},
                {name: 'value'}
            ]),
        
            defaultCheck = new Ext.ux.grid.CheckColumn({
                id: 'default', 
                header: self.app.i18n._('Default'),
                dataIndex: 'default',
                sortable: false,
                align: 'center',
                width: 55
            });
            
        this[this.fieldType + 'Store'] = new Tine.widgets.grid.QuickaddGridPanel({
            autoExpandColumn: 'value',
            quickaddMandatory: 'id',
            resetAllOnNew: true,
            useBBar: true,
            border: false,
            recordClass: storeEntry,
            store: new Ext.data.Store({
                reader: new Ext.data.ArrayReader({idIndex: 0}, storeEntry),
                listeners: {
                    scope: this,
                    'update': function (store, rec, operation) {
                        rec.commit(true);
                        
                        // be sure that only one default is checked
                        if (rec.get('default')) {
                            store.each(function (r) {
                                if (r.id !== rec.id) {
                                    r.set('default', false);
                                    r.commit(true);
                                }
                            }, this);
                        }                       
                    }
                }
            }),
            plugins: [defaultCheck],
            getColumnModel: function () {
                return new Ext.grid.ColumnModel([
                defaultCheck,
                {
                    id: 'id', 
                    header: self.app.i18n._('ID'), 
                    dataIndex: 'id', 
                    hideable: false, 
                    sortable: false, 
                    quickaddField: new Ext.form.TextField({
                        emptyText: self.app.i18n._('Add a New ID...')
                    })
                }, {
                    id: 'value', 
                    header: self.app.i18n._('Value'), 
                    dataIndex: 'value', 
                    hideable: false, 
                    sortable: false, 
                    quickaddField: new Ext.form.TextField({
                        emptyText: self.app.i18n._('Add a New Value...')
                    })
                }]);
            },
            /**
             * Do some checking on new entry add
             */
            onNewentry: function (recordData) {
                // check if id exists in grid
                if (this.store.findExact('id', recordData.id) !== -1) {
                    Ext.Msg.alert(self.app.i18n._('Error'), self.app.i18n._('ID already exists'));
                    return false;
                }
                    
                // check if value exists in grid
                if (this.store.findExact('value', recordData.value) !== -1) {
                    Ext.Msg.alert(self.app.i18n._('Error'), self.app.i18n._('Value already exists'));
                    return false;
                }
                
                // if value is empty, set it to ID
                if (Ext.isEmpty(recordData.value)) {
                    recordData.value = recordData.id;
                }
                
                Tine.widgets.grid.QuickaddGridPanel.prototype.onNewentry.apply(this, arguments);
            },
            setValue: function (data) {
                this.setStoreFromArray(data);
            },
            getValue: function () {
                var data = [];
                this.store.each(function (rec) {
                    data.push({
                        id: rec.get('id'),
                        value: rec.get('value')
                    });
                });
                
                return data;
            },
            isValid: function () {
                return true;
            }
        });
        
        /**
         * Load values if exists in field definition
         */
        var configValue = ! Ext.isEmpty(this.record.get('definition')) && this.record.get('definition')[this.fieldType + 'Config'] 
            ? this.record.get('definition')[this.fieldType + 'Config'].value 
            : null;
            
        if (this.record.id != 0 && configValue) {
            this[this.fieldType + 'Store'].setValue(configValue.records);
            
            // if there is default value check it
            if (configValue['default']) {
                var defaultRecIdx = this[this.fieldType + 'Store'].store.findExact('id', configValue['default']);
                if (defaultRecIdx !== -1) {
                    this[this.fieldType + 'Store'].store.getAt(defaultRecIdx).set('default', true);
                }
            }
        }
        
        return this[this.fieldType + 'Store'];
    },
    
    /**
     * Create store for record field
     * 
     * @returns {Ext.Panel}
     */
    initRecordStore: function () {
        Tine.log.info('Initialize record store config');
        
        var self = this;
               
        this[this.fieldType + 'Store'] = new Ext.FormPanel({
            labelAlign: 'top',
            frame: true,
            border: false,
            defaults: {anchor: '100%'},
            bodyStyle: 'padding: 15px',
            items:[{
                xtype: 'combo',
                store: this.appStore,
                name: 'application_id',
                displayField: 'name',
                valueField: 'id',
                fieldLabel: this.app.i18n._('Application'),
                mode: 'local',
                forceSelection: true,
                editable: false,
                listeners: {
                    scope: this,
                    'select': function (combo, rec) {
                        // load combo with found models for selected application
                        this[this.fieldType + 'Store'].items.get(1).store.loadData(this.getApplicationModels(rec, false));
                    }
                }
            }, {
                xtype: 'combo',
                store: new Ext.data.ArrayStore({
                    idIndex: 0,  
                    fields: ['recordClass', 'recordName']
                }),
                name: 'recordClass',
                displayField: 'recordName',
                valueField: 'recordClass',
                fieldLabel: this.app.i18n._('Record Class'),
                mode: 'local',
                forceSelection: false,
                editable: true,
                allowBlank: false
            }],
            setValue: function (data) {
                var parts = data.split('.'), // e.g Tine.Admin.Model.Group
                    app   = Tine.Tinebase.appMgr.get(parts[1]);
                
                // set value for application combo
                this.items.get(0).setValue(app.id);
                
                // load records based on application combo and set value
                this.items.get(1).store.loadData(self.getApplicationModels(app.appName, false));
                this.items.get(1).setValue(data);
                
            },
            getValue: function () {
                return this.items.get(1).getValue();
            },
            isValid: function () {
                try {
                    var model = eval(this.items.get(1).getValue());
                } catch (e) {
                    Ext.Msg.alert(_('Errors'), self.app.i18n._('Given record class not found'));
                    return false;
                }
                
                return true;
            }
        });
        
        /**
         * Load values if exists in field definition
         */
        var configValue = ! Ext.isEmpty(this.record.get('definition')) && this.record.get('definition')[this.fieldType + 'Config'] 
            ? this.record.get('definition')[this.fieldType + 'Config'].value 
            : null;
            
        if (this.record.id != 0 && configValue) {
            this[this.fieldType + 'Store'].setValue(configValue.records);
        }
        
        return this[this.fieldType + 'Store'];
    },
    
    /**
     * Show window for configuring field store
     */
    showStoreWindow: function () {
        this.storeWindow = Tine.WindowFactory.getWindow({
            modal: true, // this needs to be modal atm, popup does not work due to issues with this in tineInit.js
            width: 500,
            height: 320,
            border: false,
            items: this['init' + (this.fieldType.charAt(0).toUpperCase() + this.fieldType.substr(1)) + 'Store'](),
            fbar: ['->', {
                text: _('OK'),
                minWidth: 70,
                scope: this,
                handler: this.onStoreWindowOK,
                iconCls: 'action_applyChanges'
            }, {
                text: _('Cancel'),
                minWidth: 70,
                scope: this,
                handler: this.onStoreWindowClose,
                iconCls: 'action_cancel'
            }]
        });
    },
    
    /**
     * returns dialog
     */
    getFormItems: function () {
        this.appStore = new Ext.data.JsonStore({
            root: 'results',
            totalProperty: 'totalcount',
            fields: Tine.Admin.Model.Application
        });
        this.appStore.loadData({
            results:    Tine.Tinebase.registry.get('userApplications'),
            totalcount: Tine.Tinebase.registry.get('userApplications').length
        });
        
        this.modelStore = new Ext.data.ArrayStore({
            idIndex: 0,
            fields: [{name: 'value'}, {name: 'name'}]
        });
        
        this.configureStoreBtn = new Ext.Button({
            columnWidth: 0.333,
            fieldLabel: '&#160;',
            xtype: 'button',
            iconCls: 'admin-node-customfields-store',
            text: this.app.i18n._('Configure store'),
            disabled: this.record.id == 0 || this.fieldsWithStore.indexOf(this.record.get('definition').type) === -1,
            scope: this,
            handler: this.showStoreWindow
        });
        
        return {
            layout: 'vbox',
            layoutConfig: {
                align: 'stretch',
                pack: 'start'
            },
            border: false,
            items: [{
                xtype: 'columnform',
                border: false,
                autoHeight: true,
                items: [[{
                    xtype: 'combo',
                    readOnly: this.record.id != 0,
                    store: this.appStore,
                    columnWidth: 0.5,
                    name: 'application_id',
                    displayField: 'name',
                    valueField: 'id',
                    fieldLabel: this.app.i18n._('Application'),
                    mode: 'local',
                    anchor: '100%',
                    allowBlank: false,
                    forceSelection: true,
                    editable: false,
                    listeners: {
                        scope: this,
                        'select': function (combo, rec) {
                            // add models for select application
                            this.modelStore.loadData(this.getApplicationModels(rec, true));
                        }
                    }
                }, {
                    xtype: 'combo',
                    readOnly: this.record.id != 0,
                    store: this.modelStore,
                    columnWidth: 0.5,
                    name: 'model',
                    displayField: 'name',
                    valueField: 'value',
                    fieldLabel: this.app.i18n._('Model'),
                    mode: 'local',
                    anchor: '100%',
                    allowBlank: false,
                    forceSelection: true,
                    editable: false
                }]]
            }, {
                xtype: 'fieldset',
                bodyStyle: 'padding: 5px',
                margins: {top: 5, right: 0, bottom: 0, left: 0},
                title: this.app.i18n._('Custom field definition'),
                labelAlign: 'top',
                defaults: {anchor: '100%'},
                items: [{
                    xtype: 'columnform',
                    border: false,
                    items: [[{
                        columnWidth: 0.666,
                        xtype: 'combo',
                        readOnly: this.record.id != 0,
                        store: [
                            ['string', this.app.i18n._('Text')],
                            ['integer', this.app.i18n._('Number')],
                            ['date', this.app.i18n._('Date')],
                            ['datetime', this.app.i18n._('DateTime')],
                            ['time', this.app.i18n._('Time')],
                            ['boolean', this.app.i18n._('Boolean')],
                            ['searchcombo', this.app.i18n._('Search Combo')],
                            ['keyField', this.app.i18n._('Key Field')],
                            ['record', this.app.i18n._('Record')]
                            
                        ],
                        name: 'type',
                        fieldLabel: this.app.i18n._('Type'),
                        mode: 'local',
                        allowBlank: false,
                        editable: false,
                        forceSelection: true,
                        listeners: {
                            scope: this,
                            'select': this.onTypeComboSelect
                        }
                    }, this.configureStoreBtn]]
                }, {
                    xtype: 'textfield',
                    fieldLabel: this.app.i18n._('Name'), 
                    name: 'name',
                    allowBlank: false,
                    maxLength: 50
                }, {
                    xtype: 'textfield',
                    fieldLabel: this.app.i18n._('Label'), 
                    name: 'label',
                    allowBlank: false,
                    maxLength: 50
                }, {
                    xtype: 'numberfield',
                    fieldLabel: this.app.i18n._('Length'),
                    name: 'length',
                    disabled: this.record.id == 0 || this.record.get('definition').type !== 'string'
                }, {
                    xtype: 'checkbox',
                    fieldLabel: this.app.i18n._('Required'),
                    name: 'required'
                }]
            }, {
                xtype: 'fieldset',
                bodyStyle: 'padding: 5px',
                margins: {top: 5, right: 0, bottom: 0, left: 0},
                title: this.app.i18n._('Custom field additional properties'),
                labelAlign: 'top',
                defaults: {anchor: '100%'},
                items: [{
                    xtype: 'textfield',
                    fieldLabel: this.app.i18n._('Group'), 
                    name: 'group',
                    maxLength: 50
                }, {
                    xtype: 'numberfield',
                    fieldLabel: this.app.i18n._('Order'),
                    name: 'order'
                }]
            }, {
                xtype: 'hidden',
                name: 'value_search',
                value: 0
            }]
        };
    },
    
    /**
     * is form valid?
     * 
     * @return {Boolean}
     */
    isValid: function() {
        var result = Tine.Admin.UserEditDialog.superclass.isValid.call(this);

        if (! this.record.id && this.customFieldExists()) {
            result = false;
            this.getForm().markInvalid([{
                id: 'name',
                msg: this.app.i18n._("Customfield already exists. Please choose another name.")
            }]);
        }
        
        return result;
    },
    
    /**
     * check if customfield name already exists for this app
     * 
     * @return {Boolean}
     */
    customFieldExists: function() {
        var applicationField = this.getForm().findField('application_id'),
            store = applicationField.getStore(),
            app = store.getById(applicationField.getValue()),
            cfName = this.getForm().findField('name').getValue(),
            cfExists = false;
        
        if (app && cfName) {
            var customfieldsOfApp = Tine[app.get('name')].registry.get('customfields');
            Ext.each(customfieldsOfApp, function(cfConfig) {
                if (cfName === cfConfig.name) {
                    cfExists = true;
                }
            }, this);
        }
        
        return cfExists;
    }
});

/**
 * Customfield Edit Popup
 * 
 * @param   {Object} config
 * @return  {Ext.ux.Window}
 */
Tine.Admin.CustomfieldEditDialog.openWindow = function (config) {
    var window = Tine.WindowFactory.getWindow({
        width: 500,
        height: 500,
        name: Tine.Admin.CustomfieldEditDialog.prototype.windowNamePrefix + Ext.id(),
        contentPanelConstructor: 'Tine.Admin.CustomfieldEditDialog',
        contentPanelConstructorConfig: config
    });
    return window;
};
/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2011 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
Ext.ns('Tine.Admin.customfield');


/**
 * Customfield grid panel
 * 
 * @namespace   Tine.Admin.customfield
 * @class       Tine.Admin.customfield.GridPanel
 * @extends     Tine.widgets.grid.GridPanel
 */
Tine.Admin.customfield.GridPanel = Ext.extend(Tine.widgets.grid.GridPanel, {
    
    /**
     * @cfg
     */
    newRecordIcon: 'admin-action-add-customfield',
    recordClass: Tine.Admin.Model.Customfield,
    recordProxy: Tine.Admin.customfieldBackend,
    defaultSortInfo: {field: 'name', direction: 'ASC'},
    evalGrants: false,
    gridConfig: {
        autoExpandColumn: 'name'
    },
    
    /**
     * initComponent
     */
    initComponent: function() {
        this.gridConfig.cm = this.getColumnModel();
        this.initFilterToolbar();
        
        this.plugins = this.plugins || [];
        this.plugins.push(this.filterToolbar);
        
        Tine.Admin.customfield.GridPanel.superclass.initComponent.call(this);
    },
    
    /**
     * returns column model
     * 
     * @return Ext.grid.ColumnModel
     * @private
     */
    getColumnModel: function() {
        return new Ext.grid.ColumnModel({
            defaults: {
                sortable: true,
                hidden: true,
                resizable: true
            },
            columns: this.getColumns()
        });
    },
    
    /**
     * returns columns
     * @private
     * @return Array
     */
    getColumns: function(){
        return [
            { header: this.app.i18n._('ID'), id: 'id', dataIndex: 'id', width: 50},
            { header: this.app.i18n._('Label'), id: 'label', dataIndex: 'definition', hidden: false, width: 100, renderer: this.labelRenderer, scope: this},
            { header: this.app.i18n._('Name'), id: 'name', dataIndex: 'name', hidden: false, width: 75},
            { header: this.app.i18n._('Type'), id: 'xtype', dataIndex: 'definition', hidden: false, width: 75, renderer: this.typeRenderer, scope: this},
            { header: this.app.i18n._('Application'), id: 'application_id', dataIndex: 'application_id', hidden: false, width: 100, renderer: this.appRenderer, scope: this},
            { header: this.app.i18n._('Model'), id: 'model', dataIndex: 'model', hidden: false, width: 100}
        ];
    },
    
    /**
     * returns label name
     * 
     * @param {Object} value
     * @return {String}
     */
    labelRenderer: function(value) {
        return this.app.i18n._(value.label);
    },
    
    /**
     * returns type name
     * 
     * @param {Object} value
     * @return {String}
     */
    typeRenderer: function(value) {
        return this.app.i18n._(value.type);
    },
    
    /**
     * returns application name
     * 
     * @param {Object} value
     * @return {String}
     */
    appRenderer: function(value) {
        return this.app.i18n._(value.name);
    },
       
    /**
     * initialises filter toolbar
     */
    initFilterToolbar: function() {
        this.filterToolbar = new Tine.widgets.grid.FilterToolbar({
            filterModels: [
                {label: this.app.i18n._('Customfield'),       field: 'query',    operators: ['contains']},
                {filtertype: 'admin.application', app: this.app}
            ],
            defaultFilter: 'query',
            filters: [
            ],
            plugins: [
                new Tine.widgets.grid.FilterToolbarQuickFilterPlugin()
            ]
        });
    },
    
    /**
     * Confirm application restart
     */
    confirmApplicationRestart: function () {
        Ext.Msg.confirm(this.app.i18n._('Confirm'), this.app.i18n._('Restart application to apply new customfields?'), function (btn) {
            if (btn == 'yes') {
                // reload mainscreen to make sure registry gets updated
                window.location = window.location.href.replace(/#+.*/, '');
            }
        }, this);
    },
    
    /**
     * on update after edit
     * 
     * @param {String|Tine.Tinebase.data.Record} record
     */
    onUpdateRecord: function (record) {
        Tine.Admin.customfield.GridPanel.superclass.onUpdateRecord.apply(this, arguments);
        
        this.confirmApplicationRestart();
    },
    
    /**
     * do something after deletion of records
     * - reload the store
     * 
     * @param {Array} [ids]
     */
    onAfterDelete: function (ids) {
        Tine.Admin.customfield.GridPanel.superclass.onAfterDelete.apply(this, arguments);
        
        this.confirmApplicationRestart();
    }
});
