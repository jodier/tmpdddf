/*!
 * Tine 2.0 - Setup 
 * Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 * http://www.gnu.org/licenses/agpl.html AGPL Version 3
 */
/*
 * Tine 2.0
 * 
 * @package     Setup
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2009 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */

/*global Ext, Tine*/

Ext.ns('Tine', 'Tine.Setup');
 
/**
 * init ajax
 */
Tine.Tinebase.tineInit.initAjax = Tine.Tinebase.tineInit.initAjax.createInterceptor(function () {
    // setup calls can take quite a while
    Ext.Ajax.timeout = 900000; // 15 mins
    Tine.Tinebase.tineInit.requestUrl = 'setup.php';
    
    return true;
});

/**
 * init registry
 */
Tine.Tinebase.tineInit.initRegistry = Tine.Tinebase.tineInit.initRegistry.createInterceptor(function () {
    Tine.Tinebase.tineInit.getAllRegistryDataMethod = 'Setup.getAllRegistryData';
    Tine.Tinebase.tineInit.stateful = false;
    
    return true;
});

Tine.Tinebase.tineInit.checkSelfUpdate = Ext.emptyFn;

/**
 * render window
 */
Tine.Tinebase.tineInit.renderWindow = Tine.Tinebase.tineInit.renderWindow.createInterceptor(function () {
    var mainCardPanel = Tine.Tinebase.viewport.tineViewportMaincardpanel;
    
    // if a config file exists, the admin needs to login!        
    if (Tine.Setup.registry.get('configExists') && !Tine.Setup.registry.get('currentAccount')) {
        if (! Tine.loginPanel) {
            Tine.loginPanel = new Tine.Tinebase.LoginPanel({
                loginMethod: 'Setup.login',
                loginLogo: 'images/tine_logo_setup.png',
                scope: this,
                onLogin: function (response) {
                    Tine.Tinebase.tineInit.initList.initRegistry = false;
                    Tine.Tinebase.tineInit.initRegistry();
                    var waitForRegistry = function () {
                        if (Tine.Tinebase.tineInit.initList.initRegistry) {
                            Ext.MessageBox.hide();
                            Tine.Tinebase.tineInit.renderWindow();
                        } else {
                            waitForRegistry.defer(100);
                        }
                    };
                    waitForRegistry();
                }
            });
            mainCardPanel.layout.container.add(Tine.loginPanel);
        }
        mainCardPanel.layout.setActiveItem(Tine.loginPanel.id);
        Tine.loginPanel.doLayout();
        
        return false;
    }
        
    // fake a setup user
    var setupUser = {
        accountId           : 1,
        accountDisplayName  : Tine.Setup.registry.get('currentAccount'),
        accountLastName     : 'Admin',
        accountFirstName    : 'Setup',
        accountFullName     : 'Setup Admin'
    };
    Tine.Tinebase.registry.add('currentAccount', setupUser);
    
    // enable setup app
    Tine.Tinebase.registry.add('userApplications', [{
        name:   'Setup',
        status: 'enabled'
    }]);
    Tine.Tinebase.MainScreen.prototype.defaultAppName = 'Setup';
    Tine.Tinebase.MainScreen.prototype.appPickerStyle = 'none';
    
    return true;
});
/*
 * Tine 2.0
 * 
 * @package     Setup
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2009 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.ns('Tine', 'Tine.Setup');

/**
 * @namespace   Tine.Setup
 * @class       Tine.Setup.TreePanel
 * @extends     Ext.tree.TreePanel
 * 
 * <p>Setup TreePanel</p>
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2009 Metaways Infosystems GmbH (http://www.metaways.de)
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Setup.TreePanel
 */
Tine.Setup.TreePanel = Ext.extend(Ext.tree.TreePanel, {
    
    /**
     * tree panel cfg
     * 
     * @private
     */
    border: false,
    rootVisible: false, 
    
    /**
     * @private
     */
    initComponent: function() {
        this.app = Tine.Tinebase.appMgr.get('Setup');
        
        var termsFailed   = !Tine.Setup.registry.get('acceptedTermsVersion') || Tine.Setup.registry.get('acceptedTermsVersion') < Tine.Setup.CurrentTermsVersion;
        var testsFailed   = !Tine.Setup.registry.get('setupChecks').success;
        var configMissing = !Tine.Setup.registry.get('configExists');
        var dbMissing     = !Tine.Setup.registry.get('checkDB');
        var setupRequired = Tine.Setup.registry.get('setupRequired');
        
        this.root = {
            id: '/',
            children: [{
                text: this.app.i18n._('Terms and Conditions'),
                iconCls: termsFailed ? 'setup_checks_fail' : 'setup_checks_success',
                id: 'TermsPanel',
                leaf: true
            }, {
                text: this.app.i18n._('Setup Checks'),
                iconCls: testsFailed ? 'setup_checks_fail' : 'setup_checks_success',
                disabled: termsFailed,
                id: 'EnvCheckGridPanel',
                leaf: true
            }, {
                text: this.app.i18n._('Config Manager'),
                iconCls: 'setup_config_manager',
                disabled: termsFailed || testsFailed,
                id: 'ConfigManagerPanel',
                leaf: true
            }, {
                text: this.app.i18n._('Authentication/Accounts'),
                iconCls: 'setup_authentication_manager',
                disabled: termsFailed || testsFailed || configMissing || dbMissing,
                id: 'AuthenticationPanel',
                leaf: true
            }, {
                text: this.app.i18n._('Email'),
                iconCls: 'action_composeEmail',
                disabled: termsFailed || testsFailed || configMissing || dbMissing || setupRequired,
                id: 'EmailPanel',
                leaf: true
            }, {
                text: this.app.i18n._('Application Manager'),
                iconCls: 'setup_application_manager',
                disabled: termsFailed || testsFailed || configMissing || dbMissing || setupRequired,
                id: 'ApplicationGridPanel',
                leaf: true
            }]
        };
        
        Tine.Setup.TreePanel.superclass.initComponent.call(this);
        
        this.on('click', this.onNodeClick, this);
    },
    
    /**
     * @private
     */
    onNodeClick: function(node) {
        if (! node.disabled) {
            this.app.getMainScreen().activePanel = node.id;
            this.app.getMainScreen().show();
        } else {
            return false;
        }
        
    },
    
    /**
     * @private
     */
    afterRender: function() {
        Tine.Setup.TreePanel.superclass.afterRender.call(this);
        
        var activeType = '';
        var contentTypes = this.getRootNode().childNodes;
        for (var i=0; i<contentTypes.length; i++) {
            if(! contentTypes[i].disabled) {
                activeType = contentTypes[i];
            }
        }
        
        activeType.select();
        this.app.getMainScreen().activePanel = activeType.id;
        
        Tine.Setup.registry.on('replace', this.applyRegistryState, this);
    },
    
    /**
     * apply registry state
     */
    applyRegistryState: function() {
        var termsChecks  = Tine.Setup.registry.get('acceptedTermsVersion') >= Tine.Setup.CurrentTermsVersion;
        var setupChecks  = Tine.Setup.registry.get('setupChecks').success;
        var configExists = Tine.Setup.registry.get('configExists');
        var checkDB      = Tine.Setup.registry.get('checkDB');
        var setupRequired = Tine.Setup.registry.get('setupRequired');
        
        this.setNodeIcon('TermsPanel', termsChecks);
        this.setNodeIcon('EnvCheckGridPanel', setupChecks);
        
        this.getNodeById('EnvCheckGridPanel')[termsChecks ? 'enable': 'disable']();
        this.getNodeById('ConfigManagerPanel')[termsChecks && setupChecks ? 'enable': 'disable']();
        this.getNodeById('AuthenticationPanel')[termsChecks && setupChecks && configExists && checkDB ? 'enable': 'disable']();
        this.getNodeById('ApplicationGridPanel')[termsChecks && setupChecks && configExists && checkDB && !setupRequired ? 'enable': 'disable']();
        this.getNodeById('EmailPanel')[termsChecks && setupChecks && configExists && checkDB && !setupRequired ? 'enable': 'disable']();
    },
    
    setNodeIcon: function (nodeId, success) {
        var node = this.getNodeById(nodeId);
        var iconCls = success ? 'setup_checks_success' : 'setup_checks_fail';
        if (node.rendered) {
            var iconEl = Ext.get(node.ui.iconNode);
            iconEl.removeClass('setup_checks_success');
            iconEl.removeClass('setup_checks_fail');
            iconEl.addClass(iconCls);
        } else {
            envNode.iconCls = iconCls;
        }
    }
});

Ext.ns('Tine', 'Tine.Setup', 'Tine.Setup.Model');

/**
 * @namespace   Tine.Setup.Model
 * @class       Tine.Setup.Model.Application
 * @extends     Tine.Tinebase.data.Record
 * 
 * Application Record Definition
 */ 
Tine.Setup.Model.Application = Tine.Tinebase.data.Record.create([
    { name: 'id'              },
    { name: 'name'            },
    { name: 'status'          },
    { name: 'order'           },
    { name: 'version'         },
    { name: 'current_version' },
    { name: 'install_status'  },
    { name: 'depends'         }
], {
    appName: 'Setup',
    modelName: 'Application',
    idProperty: 'name',
    titleProperty: 'name',
    // ngettext('Application', 'Applications', n); gettext('Application');
    recordName: 'Application',
    recordsName: 'Applications'
});

/**
 * @namespace   Tine.Setup
 * @class       Tine.Setup.ApplicationBackend
 * @extends     Tine.Tinebase.data.RecordProxy
 * 
 * default application backend
 */ 
Tine.Setup.ApplicationBackend = new Tine.Tinebase.data.RecordProxy({
    appName: 'Setup',
    modelName: 'Application',
    recordClass: Tine.Setup.Model.Application
});

/**
 * @namespace   Tine.Setup.Model
 * @class       Tine.Setup.Model.EnvCheck
 * @extends     Ext.data.Record
 * 
 * env check Record Definition
 */ 
Tine.Setup.Model.EnvCheck = Ext.data.Record.create([
    {name: 'key'},
    {name: 'value'},
    {name: 'message'}
]);
/*
 * Tine 2.0
 * 
 * @package     Setup
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2009 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.ns('Tine', 'Tine.Setup');

/**
 * @namespace   Tine.Setup
 * @class       Tine.Setup.MainScreen
 * @extends     Tine.widgets.MainScreen
 * 
 * <p>MainScreen Definition</p>
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2009 Metaways Infosystems GmbH (http://www.metaways.de)
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Setup.MainScreen
 */
Tine.Setup.MainScreen = Ext.extend(Tine.widgets.MainScreen, {
    
    /**
     * active panel
     * 
     * @property activePanel
     * @type String
     */
    activePanel: 'EnvCheckGridPanel',
    
    /**
     * set content panel
     */
    showCenterPanel: function() {
        
        // which content panel?
        var panel = this.activePanel;
        
        if (! this[panel]) {
            this[panel] = new Tine.Setup[panel]({
                app: this.app
            });
        }
        
        Tine.Tinebase.MainScreen.setActiveContentPanel(this[panel], true);
        
        if (this[panel].hasOwnProperty('store')) {
            this[panel].store.load();
        }
    },
    
    /**
     * get content panel
     * 
     * @return {Ext.Panel}
     */
    getCenterPanel: function() {
        return this[this.activePanel];
    },
    
    /**
     * sets toolbar in mainscreen
     */
    showNorthPanel: function() {
        var panel = this.activePanel;
        
        if (! this[panel + 'ActionToolbar']) {
            this[panel + 'ActionToolbar'] = this[panel].actionToolbar;
        }
        
        Tine.Tinebase.MainScreen.setActiveToolbar(this[panel + 'ActionToolbar'], true);
        
        // hide and disable stuff in main menu
        Tine.Tinebase.MainScreen.getMainMenu().action_changePassword.setHidden(true);
        Tine.Tinebase.MainScreen.getMainMenu().action_showPreferencesDialog.setHidden(true);
        Tine.Tinebase.MainScreen.getMainMenu().action_editProfile.setDisabled(true);
    },
    
    /**
     * get west panel for given contentType
     * 
     * template method to be overridden by subclasses to modify default behaviour
     * 
     * @return {Ext.Panel}
     */
    getWestPanel: function() {
        if (! this.westPanel) {
            this.westPanel = new Tine.Setup.TreePanel();
        }
        
        return this.westPanel;
    }
});
/*
 * Tine 2.0
 * 
 * @package     Setup
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2009 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.ns('Tine', 'Tine.Setup');

Tine.Setup.CurrentTermsVersion = 1;

Tine.Setup.TermsPanel = Ext.extend(Ext.Panel, {
    border: false,
    layout: 'fit',
    version: 1,
    
    /**
     * @property actionToolbar
     * @type Ext.Toolbar
     */
    actionToolbar: null,
    
    getLicensePanel: function() {
        var acceptField = new Ext.form.Checkbox({
            name: 'acceptLicense',
            xtype: 'checkbox',
            boxLabel: this.app.i18n._('I have read the license agreement and accept it')
        });
        this.acceptFields.push(acceptField);
        
        return new Ext.Panel({
            autoScroll: true,
            layout: 'fit',
            title: this.app.i18n._('License Agreement'),
            bwrapCfg: {tag: 'pre'},
            autoLoad: {
                url: 'LICENSE',
                isUpload: true,
                method: 'GET',
                callback: function(el, s, response) {
                    el.update(Ext.util.Format.nl2br(response.responseText));
                }
            },
            bbar: [acceptField]
        });
    },
    
    getPrivacyPanel: function() {
        
        var acceptField = new Ext.form.Checkbox({
            name: 'acceptPrivacy',
            xtype: 'checkbox',
            boxLabel: this.app.i18n._('I have read the privacy agreement and accept it')
        });
        this.acceptFields.push(acceptField);
        
        return new Ext.Panel({
            autoScroll: true,
            layout: 'fit',
            title: this.app.i18n._('Privacy Agreement'),
            bwrapCfg: {tag: 'pre'},
            autoLoad: {
                url: 'PRIVACY',
                isUpload: true,
                method: 'GET',
                callback: function(el, s, response) {
                    el.update(Ext.util.Format.nl2br(response.responseText));
                }
            },
            bbar: [acceptField]
        });
    },
    
    initActions: function() {
        this.actionToolbar = new Ext.Toolbar({
            items: [{
                text: this.app.i18n._('Accept Terms and Conditions'),
                iconCls: 'setup_checks_success',
                handler: this.onAcceptConditions,
                scope: this
            }]
        });
    },
    
    initComponent: function() {
        this.initActions();
        
        this.acceptFields = [];
        this.items = [{
            layout: 'vbox',
            border: false,
            layoutConfig: {
                align:'stretch'
            },
            items: [{
                layout: 'fit',
                border: false,
                flex: 1,
                items: this.getLicensePanel()
            }, {
                layout: 'fit',
                border: false,
                flex: 1,
                items: this.getPrivacyPanel()
            }]
        }];
        
        this.supr().initComponent.call(this);
    },
    
    onAcceptConditions: function() {
        var isValid = true;
        
        Ext.each(this.acceptFields, function(field) {
            if (! field.getValue()) {
                field.wrap.setStyle('border-bottom', '1px solid red');
                isValid = false;
            } else {
                field.wrap.setStyle('border-bottom', 'none');
            }
        }, this);
        
        if (isValid) {
            Tine.Setup.registry.replace('acceptedTermsVersion', Tine.Setup.CurrentTermsVersion);
        }
    }
});
/*
 * Tine 2.0
 * 
 * @package     Setup
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2009-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.ns('Tine', 'Tine.Setup');

/**
 * @namespace   Tine.Setup
 * @class       Tine.Setup.ApplicationGridPanel
 * @extends     Tine.widgets.grid.GridPanel
 * 
 * <p>Application Setup Grid Panel</p>
 * <p></p>
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Setup.ApplicationGridPanel
 */
Tine.Setup.ApplicationGridPanel = Ext.extend(Tine.widgets.grid.GridPanel, {

    /**
     * @private
     */
    recordClass: Tine.Setup.Model.Application,
    recordProxy: Tine.Setup.ApplicationBackend,
    stateful: false,
    evalGrants: false,
    defaultSortInfo: {field: 'name', dir: 'ASC'},
    gridConfig: {
        autoExpandColumn: 'name'
    },
    
    /**
     * @private
     */
    initComponent: function() {
                
        this.gridConfig.columns = this.getColumns();
        
        Tine.Setup.ApplicationGridPanel.superclass.initComponent.call(this);
        
        // activate local sort
        this.store.remoteSort = false;
        
        // add selection of updateable apps after store load
        this.store.on('load', this.selectApps, this);
    },
    
    /**
     * @private
     */
    getColumns: function() {
        return  [
            {id: 'name',            width: 350, sortable: true, dataIndex: 'name',            header: this.app.i18n._("Name")}, 
            {id: 'status',          width: 70,  sortable: true, dataIndex: 'status',          header: this.app.i18n._("Enabled"),       renderer: this.enabledRenderer}, 
            {id: 'order',           width: 50,  sortable: true, dataIndex: 'order',           header: this.app.i18n._("Order")},
            {id: 'version',         width: 85,  sortable: true, dataIndex: 'version',         header: this.app.i18n._("Installed Version")},
            {id: 'current_version', width: 85,  sortable: true, dataIndex: 'current_version', header: this.app.i18n._("Available Version")},
            {id: 'install_status',  width: 70,  sortable: true, dataIndex: 'install_status',  header: this.app.i18n._("Status"),        renderer: this.upgradeStatusRenderer.createDelegate(this)},
            {id: 'depends',         width: 150, sortable: true, dataIndex: 'depends',         header: this.app.i18n._("Depends on")}
        ];
    },
    
    /**
     * @private
     */
    initActions: function() {
        this.action_installApplications = new Ext.Action({
            text: this.app.i18n._('Install application'),
            handler: this.onAlterApplications,
            actionType: 'install',
            iconCls: 'setup_action_install',
            disabled: true,
            scope: this
        });
        
        this.action_uninstallApplications = new Ext.Action({
            text: this.app.i18n._('Uninstall application'),
            handler: this.onAlterApplications,
            actionType: 'uninstall',
            iconCls: 'setup_action_uninstall',
            disabled: true,
            scope: this
        });
        
        this.action_updateApplications = new Ext.Action({
            text: this.app.i18n._('Update application'),
            handler: this.onAlterApplications,
            actionType: 'update',
            iconCls: 'setup_action_update',
            disabled: true,
            scope: this
        });
        
        this.action_gotoLogin = new Ext.Action({
            text: String.format(this.app.i18n._('Go to {0} login'), Tine.title),
            handler: this.onGotoLogin,
            iconCls: 'action_login',
            scope: this
        });
        
        this.actions = [
            this.action_installApplications,
            this.action_uninstallApplications,
            this.action_updateApplications,
            '-',
            this.action_gotoLogin
        ];
        
        this.actionToolbar = new Ext.Toolbar({
            split: false,
            height: 26,
            items: this.actions
        });
        
        this.contextMenu = new Ext.menu.Menu({
            items: this.actions
        });
    },
    
    /**
     * @private
     */
    initGrid: function() {
        Tine.Setup.ApplicationGridPanel.superclass.initGrid.call(this);
        this.selectionModel.purgeListeners();
        
        this.selectionModel.on('selectionchange', this.onSelectionChange, this);
    },

    /**
     * @private
     */
    onSelectionChange: function(sm) {
        var apps = sm.getSelections();
        var disabled = sm.getCount() == 0;
        
        var nIn = disabled, nUp = disabled, nUn = disabled,        
            addressbook, admin, tinebase;
        
        for(var i=0; i<apps.length; i++) {
            var status = apps[i].get('install_status');
            nIn = nIn || status == 'uptodate' || status == 'updateable';
            nUp = nUp || status == 'uptodate' || status == 'uninstalled';
            nUn = nUn || status == 'uninstalled';
            if (apps[i].id == 'Addressbook') addressbook = true;
            else if (apps[i].id == 'Tinebase') tinebase = true;
            else if (apps[i].id == 'Admin') admin = true;
        }
        
        if(this.store.getById('Tinebase').get('install_status') == 'uninstalled') tinebase = false;
        if((addressbook || admin ) && !tinebase) nUn = true;
        
        this.action_installApplications.setDisabled(nIn);
        this.action_uninstallApplications.setDisabled(nUn);
        this.action_updateApplications.setDisabled(nUp);
    },
    
    /**
     * @private
     */
    onAlterApplications: function(btn, e) {

        if (btn.actionType == 'uninstall') {
            // get user confirmation before uninstall
            Ext.Msg.confirm(this.app.i18n._('uninstall'), this.app.i18n._('Do you really want to uninstall the application(s)?'), function(confirmbtn, value) {
                if (confirmbtn == 'yes') {
                    this.alterApps(btn.actionType);
                }
            }, this);
        } else {
            this.alterApps(btn.actionType);
        }
    },
    
    /**
     * goto tine 2.0 login screen
     * 
     * @param {Button} btn
     * @param {Event} e
     */
    onGotoLogin: function(btn, e) {
        window.location = window.location.href.replace(/setup(\.php)*/, '');
    },
    
    /**
     * select all installable or updateable apps
     * @private
     */
    selectApps: function() {
        
        var updateable = [];
        
        this.store.each(function(record) {
            if (record.get('install_status') == 'updateable') {
                updateable.push(record);
            }
        }, this);
        
        this.selectionModel.selectRecords(updateable);
    },
    
    /**
     * alter applications
     * 
     * @param {} type (uninstall/install/update)
     * @private
     */
    alterApps: function(type) {

        var appNames = [];
        var apps = this.selectionModel.getSelections();
        
        for(var i=0; i<apps.length; i++) {
            appNames.push(apps[i].get('name'));
        }

        this.sendAlterApplicationsRequest(type, appNames, null);
    },
    
    /**
     * @private
     */
    sendAlterApplicationsRequest: function(type, appNames, options) {
        var msg = this.app.i18n.n_('Updating Application "{0}".', 'Updating {0} Applications.', appNames.length);
        msg = String.format(msg, appNames.length == 1 ? appNames[0] : appNames.length ) + ' ' + this.app.i18n._('This may take a while');

        var longLoadMask = new Ext.LoadMask(this.grid.getEl(), {
            msg: msg,
            removeMask: true
        });
        longLoadMask.show();
        Ext.Ajax.request({
            scope: this,
            params: {
                method: 'Setup.' + type + 'Applications',
                applicationNames: appNames,
                options: options
            },
            success: function(response) {
                var regData = Ext.util.JSON.decode(response.responseText);
                // replace some registry data
                for (key in regData) {
                    if (key != 'status' && key != 'success') {
                        Tine.Setup.registry.replace(key, regData[key]);
                    }
                }
                this.store.load();
                longLoadMask.hide();
            },
            failure: function(exception) {
                longLoadMask.hide();
                
                switch (exception.code) {
                    //Dependency Exception
                    case 501:
                        Ext.MessageBox.show({
                            title: this.app.i18n._('Dependency Violation'), 
                            msg: data.msg,
                            buttons: Ext.Msg.OK,
                            icon: Ext.MessageBox.WARNING
                        });
                        this.store.load();
                        break;
                    default:
                        Tine.Tinebase.ExceptionHandler.handleRequestException(exception);
                }
            }
        });
    },
    
    /**
     * @private
     */
    enabledRenderer: function(value) {
        return Tine.Tinebase.common.booleanRenderer(value == 'enabled');
    },
    
    /**
     * @private
     */
    upgradeStatusRenderer: function(value) {
        return this.app.i18n._hidden(value);
    }
});
/*
 * Tine 2.0
 * 
 * @package     Setup
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2009-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.ns('Tine', 'Tine.Setup');

/**
 * Environment Check Grid Panel
 * 
 * @namespace   Tine.Setup
 * @class       Tine.Setup.EnvCheckGridPanel
 * @extends     Ext.Panel
 * 
 * <p>Environment Check Grid Panel</p>
 * <p><pre>
 * </pre></p>
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schuele <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2009 Metaways Infosystems GmbH (http://www.metaways.de)
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Setup.EnvCheckGridPanel
 */
 Tine.Setup.EnvCheckGridPanel = Ext.extend(Ext.Panel, {
    
    /**
     * @property actionToolbar
     * @type Ext.Toolbar
     */
    actionToolbar: null,
    
    /**
     * @property contextMenu
     * @type Ext.Menu
     */
    contextMenu: null,
    
    /**
     * @private
     */
    layout: 'border',
    border: false,
    
    /**
     * 
     * @cfg grid config 
     */
    gridConfig: {
        autoExpandColumn: 'key'
    },
    
    /**
     * init component
     */
    initComponent: function() {
        
        this.gridConfig.columns = this.getColumns();
        
        this.initActions();
        this.initStore();
        this.initGrid();
        this.initLayout();

        Tine.Setup.EnvCheckGridPanel.superclass.initComponent.call(this);
    },
    
    /**
     * init store
     * @private
     */
    initStore: function() {
        this.store = new Ext.data.JsonStore({
            fields: Tine.Setup.Model.EnvCheck,
            mode: 'local',
            id: 'key',
            remoteSort: false
        });
        
        this.store.on('beforeload', function() {
            if (! this.loadMask) {
                this.loadMask = new Ext.LoadMask(this.el, {msg: this.app.i18n._("Performing Environment Checks...")});
            }
            
            this.loadMask.show();
            
            Ext.Ajax.request({
                params: {
                    method: 'Setup.envCheck'
                },
                scope: this,
                success: function(response) {
                    var data = Ext.util.JSON.decode(response.responseText);
                    Tine.Setup.registry.replace('setupChecks', data);
                    
                    this.store.loadData(data.results);
                    this.loadMask.hide();
                }
            })
            
            return false;
        }, this);
        
        var checkData = Tine.Setup.registry.get('setupChecks').results;
        this.store.loadData(checkData);
    },

    /**
     * init ext grid panel
     * @private
     */
    initGrid: function() {
        // init sel model
        this.selectionModel = new Ext.grid.RowSelectionModel({
            store: this.store
        });
        
        // init view
        var view =  new Ext.grid.GridView({
            autoFill: true,
            forceFit:true,
            ignoreAdd: true
            //emptyText: String.format(Tine.Tinebase.translation._("There could not be found any {0}. Please try to change your filter-criteria, view-options or the {1} you search in."), this.i18nRecordsName, this.i18nContainersName),
            /*
            onLoad: Ext.emptyFn,
            listeners: {
                beforerefresh: function(v) {
                    v.scrollTop = v.scroller.dom.scrollTop;
                },
                refresh: function(v) {
                    // on paging-refreshes (prev/last...) we don't preserv the scroller state
                    if (v.isPagingRefresh) {
                        v.scrollToTop();
                        v.isPagingRefresh = false;
                    } else {
                        v.scroller.dom.scrollTop = v.scrollTop;
                    }
                }
            }
            */
        });
        
        this.grid = new Ext.grid.GridPanel(Ext.applyIf(this.gridConfig, {
            border: false,
            store: this.store,
            sm: this.selectionModel,
            view: view
        }));
    },
    
    getColumns: function() {
        return  [
            {id: 'key',   width: 150, sortable: true, dataIndex: 'key',   header: this.app.i18n._("Check")}, 
            {id: 'value', width: 50, sortable: true, dataIndex: 'value', header: this.app.i18n._("Result"), renderer: this.resultRenderer},
            {id: 'message', width: 600, sortable: true, dataIndex: 'message', header: this.app.i18n._("Error"), renderer: this.messageRenderer}
        ];
    },

    resultRenderer: function(value) {
        var icon = (value) ? 'images/oxygen/16x16/actions/dialog-apply.png' : 'images/oxygen/16x16/actions/dialog-cancel.png';
        return '<img class="TasksMainGridStatus" src="' + icon + '">';
    },
    
    messageRenderer: function(value) {
        // overwrite the default renderer to show links correctly
        return value;
    },
    
    initActions: function() {
        // @todo add re-run checks here
        
        this.action_reCheck = new Ext.Action({
            text: this.app.i18n._('Run setup tests'),
            handler: function() {
                this.store.load({});
            },
            iconCls: 'x-tbar-loading',
            scope: this
        });
        
        this.action_ignoreTests = new Ext.Action({
            text: this.app.i18n._('Ignore setup tests'),
            // we are not ready for this button yet:
            //    setup only works with mysql version check ok
            disabled: true,
            iconCls: 'setup_checks_success',
            scope: this,
            handler: function() {
                var checks = Tine.Setup.registry.get('setupChecks');
                checks.success = true;
                Tine.Setup.registry.replace('setupChecks', checks);
                Tine.Setup.registry.replace('checkDB', true);
            }
        });
        /*
        this.action_installApplications = new Ext.Action({
            text: this.app.i18n._('Install application'),
            handler: this.onAlterApplications,
            actionType: 'install',
            iconCls: 'setup_action_install',
            disabled: true,
            scope: this
        });
        
        this.actions = [
            this.action_installApplications,
        ];
        */
        
        this.actionToolbar = new Ext.Toolbar({
            items: [
                this.action_reCheck,
                this.action_ignoreTests
            ]
        });
    },
    
    /**
     * @private
     * 
     * NOTE: Order of items matters! Ext.Layout.Border.SplitRegion.layout() does not
     *       fence the rendering correctly, as such it's impotant, so have the ftb
     *       defined after all other layout items
     */
    initLayout: function() {
        this.items = [{
            region: 'center',
            xtype: 'panel',
            layout: 'fit',
            border: false,
            items: this.grid
        }];
    }
});
/*
 * Tine 2.0
 * 
 * @package     Setup
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2009-2012 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */

/*global Ext, Tine*/

Ext.ns('Tine', 'Tine.Setup');

/**
 * Setup Configuration Manager
 * 
 * @namespace   Tine.Setup
 * @class       Tine.Setup.ConfigManagerPanel
 * @extends     Tine.Tinebase.widgets.form.ConfigPanel
 * 
 * <p>Configuration Panel</p>
 * <p><pre>
 * TODO         add cache backend config(s)
 * </pre></p>
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Setup.ConfigManagerPanel
 */
Tine.Setup.ConfigManagerPanel = Ext.extend(Tine.Tinebase.widgets.form.ConfigPanel, {

    /**
     * @property idPrefix DOM Id prefix
     * @type String
     */
    idPrefix: null,
    
    /**
     * @private
     * panel cfg
     */
    saveMethod: 'Setup.saveConfig',
    registryKey: 'configData',
    defaults: {
        xtype: 'fieldset',
        autoHeight: 'auto',
        defaults: {width: 300},
        defaultType: 'textfield'
    },
    
    /**
     * session backend DOM Id prefix
     * 
     * @property sessionBackendIdPrefix
     * @type String
     */
    sessionBackendIdPrefix: null,
    
    /**
     * @private
     * field index counter
     */
    tabIndexCounter: 1,
    
    /**
     * @private
     */
    initComponent: function () {
        this.idPrefix                  = Ext.id();
        this.sessionBackendIdPrefix    = this.idPrefix + '-sessionBackend-';

        Tine.Setup.ConfigManagerPanel.superclass.initComponent.call(this);
    },
    
    /**
     * Change session card layout depending on selected combo box entry
     */
    onChangeSessionBackend: function () {
        this.changeCard(this.sessionBackendCombo, this.sessionBackendIdPrefix);
    },
    
    /**
     * Change default ports when database adapter gets changed
     */
    onChangeSqlBackend: function () {
        // @todo change default port
    },
    
    /**
     * @private
     */
    onRender: function (ct, position) {
        Tine.Setup.EmailPanel.superclass.onRender.call(this, ct, position);
        
        this.onChangeSessionBackend.defer(250, this);
    },
    
    /**
     * get tab index for field
     * 
     * @return {Integer}
     */
    getTabIndex: function () {
        return this.tabIndexCounter++;
    },
    
    /**
     * returns config manager form
     * 
     * @private
     * @return {Array} items
     */
    getFormItems: function () {
        // common config for all combos in this setup
        var commonComboConfig = {
            xtype: 'combo',
            listWidth: 300,
            mode: 'local',
            forceSelection: true,
            allowEmpty: false,
            triggerAction: 'all',
            editable: false,
            tabIndex: this.getTabIndex
        };
        
        this.sessionBackendCombo = new Ext.form.ComboBox(Ext.applyIf({
            name: 'session_backend',
            fieldLabel: this.app.i18n._('Backend'),
            value: 'File',
            // TODO add redis again when we are ready
            store: [['File', this.app.i18n._('File')]/*, ['Redis','Redis'] */],
            listeners: {
                scope: this,
                change: this.onChangeSessionBackend,
                select: this.onChangeSessionBackend
            }
        }, commonComboConfig));
        
        this.sqlBackendCombo = new Ext.form.ComboBox(Ext.applyIf({
            name: 'database_adapter',
            fieldLabel: this.app.i18n._('Adapter'),
            value: 'pdo_mysql',
            store: [
                ['pdo_mysql', 'MySQL'],
                ['pdo_pgsql', 'PostgreSQL'],
                ['oracle', 'Oracle']
            ],
            listeners: {
                scope: this,
                change: this.onChangeSqlBackend,
                select: this.onChangeSqlBackend
            }
        }, commonComboConfig));
        
        return [{
            title: this.app.i18n._('Setup Authentication'),
            defaults: {
                width: 300,
                tabIndex: this.getTabIndex
            },
            items: [{
                name: 'setupuser_username',
                fieldLabel: this.app.i18n._('Username'),
                allowBlank: false,
                listeners: {
                    afterrender: function (field) {
                        field.focus(true, 500);
                    }
                }
            }, {
                name: 'setupuser_password',
                fieldLabel: this.app.i18n._('Password'),
                inputType: 'password',
                allowBlank: false
            }] 
        }, {
            title: this.app.i18n._('Database'),
            id: 'setup-database-group',
            defaults: {
                width: 300,
                tabIndex: this.getTabIndex
            },
            items: [ 
                this.sqlBackendCombo, 
                {
                    name: 'database_host',
                    fieldLabel: this.app.i18n._('Hostname'),
                    allowBlank: false
                }, {
                    xtype: 'numberfield',
                    name: 'database_port',
                    fieldLabel: this.app.i18n._('Port')
                }, {
                    name: 'database_dbname',
                    fieldLabel: this.app.i18n._('Database'),
                    allowBlank: false
                }, {
                    name: 'database_username',
                    fieldLabel: this.app.i18n._('User'),
                    allowBlank: false
                }, {
                    name: 'database_password',
                    fieldLabel: this.app.i18n._('Password'),
                    inputType: 'password'
                }, {
                    name: 'database_tableprefix',
                    fieldLabel: this.app.i18n._('Prefix')
                }
            ]
        }, {
            title: this.app.i18n._('Logging'),
            id: 'setup-logger-group',
            checkboxToggle: true,
            collapsed: true,
            defaults: {
                width: 300,
                tabIndex: this.getTabIndex
            },
            items: [{
                name: 'logger_filename',
                fieldLabel: this.app.i18n._('Filename')
            }, Ext.applyIf({
                name: 'logger_priority',
                fieldLabel: this.app.i18n._('Priority'),
                store: [[0, 'Emergency'], [1, 'Alert'], [2, 'Critical'], [3, 'Error'], [4, 'Warning'], [5, 'Notice'], [6, 'Informational'], [7, 'Debug'], [8, 'Trace']]
            }, commonComboConfig)]
        }, {
            title: this.app.i18n._('Caching'),
            id: 'setup-caching-group',
            checkboxToggle: true,
            collapsed: true,
            defaults: {
                width: 300,
                tabIndex: this.getTabIndex
            },
            items: [{
                name: 'caching_path',
                fieldLabel: this.app.i18n._('Path')
            }, {
                xtype: 'numberfield',
                name: 'caching_lifetime',
                fieldLabel: this.app.i18n._('Lifetime (seconds)'),
                minValue: 0,
                maxValue: 3600
            }]
        }, {
            title: this.app.i18n._('Temporary files'),
            id: 'setup-tmpDir-group',
            defaults: {
                width: 300,
                tabIndex: this.getTabIndex
            },
            items: [{
                name: 'tmpdir',
                fieldLabel: this.app.i18n._('Temporary Files Path'),
                value: Tine.Setup.registry.get(this.registryKey).tmpdir
            }]
        }, {
            title: this.app.i18n._('Session'),
            id: 'setup-session-group',
            defaults: {
                width: 300,
                tabIndex: this.getTabIndex
            },
            items: [{
                name: 'session_lifetime',
                fieldLabel: this.app.i18n._('Lifetime (seconds)'),
                xtype: 'numberfield',
                value: 86400, 
                minValue: 0
            }, this.sessionBackendCombo, 
            {
                id: this.sessionBackendIdPrefix + 'CardLayout',
                xtype: 'panel',
                layout: 'card',
                activeItem: this.sessionBackendIdPrefix + 'File',
                border: false,
                width: '100%',
                defaults: {border: false},
                items: [{
                    // file config options
                    id: this.sessionBackendIdPrefix + 'File',
                    layout: 'form',
                    autoHeight: 'auto',
                    defaults: {
                        width: 300,
                        xtype: 'textfield',
                        tabIndex: this.getTabIndex
                    },
                    items: [{
                        name: 'session_path',
                        fieldLabel: this.app.i18n._('Path')
                    }]
                }, {
                    // redis config options
                    id: this.sessionBackendIdPrefix + 'Redis',
                    layout: 'form',
                    autoHeight: 'auto',
                    defaults: {
                        width: 300,
                        xtype: 'textfield',
                        tabIndex: this.getTabIndex
                    },
                    items: [{
                        name: 'session_host',
                        fieldLabel: this.app.i18n._('Hostname'),
                        value: 'localhost'
                    }, {
                        name: 'session_port',
                        fieldLabel: this.app.i18n._('Port'),
                        xtype: 'numberfield',
                        minValue: 0,
                        value: 6379
                    }]
                }]
            }]
        }, {
            // TODO this should be not saved in the config.inc.php
            title: this.app.i18n._('Filestore directory'),
            id: 'setup-filesDir-group',
            defaults: {
                width: 300,
                tabIndex: this.getTabIndex
            },
            items: [{
                name: 'filesdir',
                fieldLabel: this.app.i18n._('Filestore Path'),
                value: Tine.Setup.registry.get(this.registryKey)['filesdir']
            }]
        }, {
            // TODO move map panel config to common config panel -> it should not be saved in config.inc.php
            title: this.app.i18n._('Addressbook Map panel'),
            defaults: {
                width: 300,
                tabIndex: this.getTabIndex
            },
            items: [
                Ext.applyIf({
                    name: 'mapPanel',
                    fieldLabel: this.app.i18n._('Map panel'),
                    value: Tine.Setup.registry.get(this.registryKey)['mapPanel'],
                    store: [[0, this.app.i18n._('disabled')], [1,this.app.i18n._('enabled')]]
                }, commonComboConfig)
            ] 
        }];
    },
    
    /**
     * applies registry state to this cmp
     */
    applyRegistryState: function () {
        this.action_saveConfig.setDisabled(! Tine.Setup.registry.get('configWritable'));
        if (! Tine.Setup.registry.get('configWritable')) {
            this.action_saveConfig.setText(this.app.i18n._('Config file is not writable'));
        } else {
            this.action_saveConfig.setText(this.app.i18n._('Save config'));
        }
        
        Ext.getCmp('setup-database-group').setIconClass(Tine.Setup.registry.get('checkDB') ? 'setup_checks_success' : 'setup_checks_fail');
        Ext.getCmp('setup-logger-group').setIconClass(Tine.Setup.registry.get('checkLogger') ? 'setup_checks_success' : 'setup_checks_fail');
        Ext.getCmp('setup-caching-group').setIconClass(Tine.Setup.registry.get('checkCaching') ? 'setup_checks_success' : 'setup_checks_fail');
        Ext.getCmp('setup-tmpDir-group').setIconClass(Tine.Setup.registry.get('checkTmpDir') ? 'setup_checks_success' : 'setup_checks_fail');
        Ext.getCmp('setup-session-group').setIconClass(Tine.Setup.registry.get('checkSessionDir') ? 'setup_checks_success' : 'setup_checks_fail');
        Ext.getCmp('setup-filesDir-group').setIconClass(Tine.Setup.registry.get('checkFilesDir') ? 'setup_checks_success' : 'setup_checks_fail');
    },
    
    /**
     * @private
     */
    initActions: function () {
        this.action_downloadConfig = new Ext.Action({
            text: this.app.i18n._('Download config file'),
            iconCls: 'setup_action_download_config',
            scope: this,
            handler: this.onDownloadConfig
            //disabled: true
        });
        
        this.actionToolbarItems = [this.action_downloadConfig];
        
        Tine.Setup.ConfigManagerPanel.superclass.initActions.apply(this, arguments);
    },
    
    onDownloadConfig: function() {
        if (this.isValid()) {
            var configData = this.form2config();
            
            var downloader = new Ext.ux.file.Download({
                url: Tine.Tinebase.tineInit.requestUrl,
                params: {
                    method: 'Setup.downloadConfig',
                    data: Ext.encode(configData)
                }
            });
            downloader.start();
        } else {
            this.alertInvalidData();
        }
    }
});/*
 * Tine 2.0
 * 
 * @package     Setup
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2009-2012 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */

/*global Ext, Tine*/

Ext.ns('Tine', 'Tine.Setup');
 
/**
 * Setup Authentication Manager
 * 
 * @namespace   Tine.Setup
 * @class       Tine.Setup.AuthenticationPanel
 * @extends     Tine.Tinebase.widgets.form.ConfigPanel
 * 
 * <p>Authentication Panel</p>
 * <p><pre>
 * TODO         move to next step after install?
 * TODO         make default is valid mechanism with 'allowEmpty' work
 * TODO         add port for ldap hosts
 * </pre></p>
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2009 Metaways Infosystems GmbH (http://www.metaways.de)
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Setup.AuthenticationPanel
 */
Tine.Setup.AuthenticationPanel = Ext.extend(Tine.Tinebase.widgets.form.ConfigPanel, {
    
    /**
     * @property idPrefix DOM Id prefix
     * @type String
     */
    idPrefix: null,
    
    /**
     * authProviderPrefix DOM Id prefix
     * 
     * @property authProviderIdPrefix
     * @type String
     */
    authProviderIdPrefix: null,
    
    /**
     * accountsStorageIdPrefix DOM Id prefix
     * 
     * @property accountsStorageIdPrefix
     * @type String
     */
    accountsStorageIdPrefix: null,
    
    /**
     * combo box containing the authentication backend selection
     * 
     * @property authenticationBackendCombo
     * @type Ext.form.ComboBox 
     */
    authenticationBackendCombo: null,

    /**
     * combo box containing the accounts storage selection
     * 
     * @property accountsStorageCombo
     * @type Ext.form.ComboBox
     */
    accountsStorageCombo: null,
    
    /**
     * The currently active accounts storage backend
     * 
     * @property originalAccountsStorage
     * @type String
     */
    originalAccountsStorage: null,

    /**
     * @private
     * panel cfg
     */
    saveMethod: 'Setup.saveAuthentication',
    registryKey: 'authenticationData',
    
    /**
     * @private
     * field index counter
     */
    tabIndexCounter: 1,
    
    /**
     * @private
     */
    initComponent: function () {
        this.idPrefix                   = Ext.id();
        this.authProviderIdPrefix       = this.idPrefix + '-authProvider-';
        this.accountsStorageIdPrefix    = this.idPrefix + '-accountsStorage-';
        this.originalAccountsStorage    = (Tine.Setup.registry.get(this.registryKey).accounts) ? Tine.Setup.registry.get(this.registryKey).accounts.backend : 'Sql';
        
        Tine.Setup.AuthenticationPanel.superclass.initComponent.call(this);
    },
    
    /**
     * Change card layout depending on selected combo box entry
     */
    onChangeAuthProvider: function () {
        var authProvider = this.authenticationBackendCombo.getValue();
        
        var cardLayout = Ext.getCmp(this.authProviderIdPrefix + 'CardLayout').getLayout();
        cardLayout.setActiveItem(this.authProviderIdPrefix + authProvider);
    },
    
    /**
     * Change card layout depending on selected combo box entry
     */
    onChangeAccountsStorage: function () {
        var AccountsStorage = this.accountsStorageCombo.getValue();

        if (AccountsStorage === 'Ldap' && AccountsStorage !== this.originalAccountsStorage) {
            Ext.Msg.confirm(this.app.i18n._('Delete all existing users and groups'), this.app.i18n._('Switching from SQL to LDAP will delete all existing User Accounts, Groups and Roles. Do you really want to switch the accounts storage backend to LDAP ?'), function (confirmbtn, value) {
                if (confirmbtn === 'yes') {
                    this.doOnChangeAccountsStorage(AccountsStorage);
                } else {
                    this.accountsStorageCombo.setValue(this.originalAccountsStorage);
                }
            }, this);
        } else {
            this.doOnChangeAccountsStorage(AccountsStorage);
        }
    },
    
    /**
     * Change card layout depending on selected combo box entry
     */
    doOnChangeAccountsStorage: function (AccountsStorage) {
        var cardLayout = Ext.getCmp(this.accountsStorageIdPrefix + 'CardLayout').getLayout();
        
        cardLayout.setActiveItem(this.accountsStorageIdPrefix + AccountsStorage);
        this.originalAccountsStorage = AccountsStorage;
    },
    
    /**
     * @private
     */
    onRender: function (ct, position) {
        Tine.Setup.AuthenticationPanel.superclass.onRender.call(this, ct, position);
        
        this.onChangeAuthProvider.defer(250, this);
        this.onChangeAccountsStorage.defer(250, this);
    },
        
    /**
     * transforms form data into a config object
     * 
     * @hack   smuggle termsAccept in 
     * @return {Object} configData
     */
    form2config: function () {
        var configData = this.supr().form2config.call(this);
        configData.acceptedTermsVersion = Tine.Setup.registry.get('acceptedTermsVersion');
        
        return configData;
    },
    
    /**
     * get tab index for field
     * 
     * @return {Integer}
     */
    getTabIndex: function () {
        return this.tabIndexCounter++;
    },
    
   /**
     * returns config manager form
     * 
     * @private
     * @return {Array} items
     */
    getFormItems: function () {
        var setupRequired = Tine.Setup.registry.get('setupRequired');
                
        // common config for all combos in this setup
        var commonComboConfig = {
            xtype: 'combo',
            listWidth: 300,
            mode: 'local',
            forceSelection: true,
            allowEmpty: false,
            triggerAction: 'all',
            editable: false,
            tabIndex: this.getTabIndex
        };
        
        this.authenticationBackendCombo = new Ext.form.ComboBox(Ext.applyIf({
            name: 'authentication_backend',
            fieldLabel: this.app.i18n._('Backend'),
            store: [['Sql', 'Sql'], ['Ldap', 'Ldap'], ['Imap', 'IMAP']],
            value: 'Sql',
            width: 300,
            listeners: {
                scope: this,
                change: this.onChangeAuthProvider,
                select: this.onChangeAuthProvider
            }
        }, commonComboConfig));
            
        this.accountsStorageCombo = new Ext.form.ComboBox(Ext.applyIf({
            name: 'accounts_backend',
            fieldLabel: this.app.i18n._('Backend'),
            store: [['Sql', 'Sql'], ['Ldap', 'Ldap']],
            value: 'Sql',
            width: 300,
            listeners: {
                scope: this,
                change: this.onChangeAccountsStorage,
                select: this.onChangeAccountsStorage
            }
        }, commonComboConfig));
        
        return [{
            xtype: 'fieldset',
            collapsible: true,
            collapsed: !setupRequired,
            autoHeight: true,
            title: this.app.i18n._('Initial Admin User'),
            items: [{
                layout: 'form',
                autoHeight: 'auto',
                border: false,
                defaults: {
                    width: 300,
                    xtype: 'textfield',
                    inputType: 'password',
                    tabIndex: this.getTabIndex
                },
                items: [{
                    name: 'authentication_Sql_adminLoginName',
                    fieldLabel: this.app.i18n._('Initial admin login name'),
                    inputType: 'text',
                    disabled: !setupRequired
                }, {
                    name: 'authentication_Sql_adminPassword',
                    fieldLabel: this.app.i18n._('Initial admin Password'),
                    disabled: !setupRequired
                }, {
                    name: 'authentication_Sql_adminPasswordConfirmation',
                    fieldLabel: this.app.i18n._('Password confirmation'),
                    disabled: !setupRequired
                }]
            }]
        }, {
            xtype: 'fieldset',
            collapsible: false,
            autoHeight: true,
            title: this.app.i18n._('Authentication provider'),
            items: [
                this.authenticationBackendCombo,
                {
                    id: this.authProviderIdPrefix + 'CardLayout',
                    layout: 'card',
                    activeItem: this.authProviderIdPrefix + 'Sql',
                    border: false,
                    defaults: {border: false},
                    items: [{
                        id: this.authProviderIdPrefix + 'Sql',
                        layout: 'form',
                        autoHeight: 'auto',
                        defaults: {
                            width: 300,
                            xtype: 'textfield',
                            tabIndex: this.getTabIndex
                        },
                        items: [
                            Ext.applyIf({
                                name: 'authentication_Sql_tryUsernameSplit',
                                fieldLabel: this.app.i18n._('Try to split username'),
                                store: [['1', this.app.i18n._('Yes')], ['0', this.app.i18n._('No')]],
                                value: '1'
                            }, commonComboConfig), 
                            Ext.applyIf({
                                name: 'authentication_Sql_accountCanonicalForm',
                                fieldLabel: this.app.i18n._('Account canonical form'),
                                store: [['2', 'ACCTNAME_FORM_USERNAME'], ['3', 'ACCTNAME_FORM_BACKSLASH'], ['4', 'ACCTNAME_FORM_PRINCIPAL']],
                                value: '2'
                            }, commonComboConfig), 
                            {
                                name: 'authentication_Sql_accountDomainName',
                                fieldLabel: this.app.i18n._('Account domain name ')
                            }, {
                                name: 'authentication_Sql_accountDomainNameShort',
                                fieldLabel: this.app.i18n._('Account domain short name')
                            }
                        ]
                    }, {
                        id: this.authProviderIdPrefix + 'Ldap',
                        layout: 'form',
                        autoHeight: 'auto',
                        defaults: {
                            width: 300,
                            xtype: 'textfield',
                            tabIndex: this.getTabIndex
                        },
                        items: [{
                            name: 'authentication_Ldap_host',
                            fieldLabel: this.app.i18n._('Host')
                        }/*, {
                            inputType: 'text',
                            name: 'authentication_Ldap_port',
                            fieldLabel: this.app.i18n._('Port')
                        }*/, {
                            name: 'authentication_Ldap_username',
                            fieldLabel: this.app.i18n._('Login name')
                        }, {
                            name: 'authentication_Ldap_password',
                            fieldLabel: this.app.i18n._('Password'),
                            inputType: 'password'
                        }, 
                        Ext.applyIf({
                            name: 'authentication_Ldap_bindRequiresDn',
                            fieldLabel: this.app.i18n._('Bind requires DN'),
                            store: [['1', this.app.i18n._('Yes')], ['0', this.app.i18n._('No')]],
                            value: '1'
                        }, commonComboConfig), 
                        {
                            name: 'authentication_Ldap_baseDn',
                            fieldLabel: this.app.i18n._('Base DN')
                        }, {
                            name: 'authentication_Ldap_accountFilterFormat',
                            fieldLabel: this.app.i18n._('Search filter')
                        }, 
                        Ext.applyIf({
                            name: 'authentication_Ldap_accountCanonicalForm',
                            fieldLabel: this.app.i18n._('Account canonical form'),
                            store: [['2', 'ACCTNAME_FORM_USERNAME'], ['3', 'ACCTNAME_FORM_BACKSLASH'], ['4', 'ACCTNAME_FORM_PRINCIPAL']],
                            value: '2'
                        }, commonComboConfig), 
                        {
                            name: 'authentication_Ldap_accountDomainName',
                            fieldLabel: this.app.i18n._('Account domain name ')
                        }, {
                            name: 'authentication_Ldap_accountDomainNameShort',
                            fieldLabel: this.app.i18n._('Account domain short name')
                        }]
                    }, {
                        id: this.authProviderIdPrefix + 'Imap',
                        layout: 'form',
                        autoHeight: 'auto',
                        defaults: {
                            width: 300,
                            xtype: 'textfield',
                            tabIndex: this.getTabIndex
                        },
                        items: [{
                            name: 'authentication_Imap_host',
                            fieldLabel: this.app.i18n._('Hostname')
                        }, {
                            name: 'authentication_Imap_port',
                            fieldLabel: this.app.i18n._('Port'),
                            xtype: 'numberfield'
                        }, 
                        Ext.applyIf({
                            fieldLabel: this.app.i18n._('Secure Connection'),
                            name: 'authentication_Imap_ssl',
                            store: [['none', this.app.i18n._('None')], ['tls', this.app.i18n._('TLS')], ['ssl', this.app.i18n._('SSL')]],
                            value: 'none'
                        }, commonComboConfig), 
                        {
                            name: 'authentication_Imap_domain',
                            fieldLabel: this.app.i18n._('Append domain to login name')
                        }
    //                    {
    //                        inputType: 'text',
    //                        xtype: 'combo',
    //                        width: 300,
    //                        listWidth: 300,
    //                        mode: 'local',
    //                        forceSelection: true,
    //                        allowEmpty: false,
    //                        triggerAction: 'all',
    //                        selectOnFocus:true,
    //                        store: [['1', 'Yes'], ['0','No']],
    //                        name: 'authentication_Sql_tryUsernameSplit',
    //                        fieldLabel: this.app.i18n._('Try to split username'),
    //                        value: '1'
    //                    }, {
    //                        inputType: 'text',
    //                        xtype: 'combo',
    //                        width: 300,
    //                        listWidth: 300,
    //                        mode: 'local',
    //                        forceSelection: true,
    //                        allowEmpty: false,
    //                        triggerAction: 'all',
    //                        selectOnFocus:true,
    //                        store: [['2', 'ACCTNAME_FORM_USERNAME'], ['3','ACCTNAME_FORM_BACKSLASH'], ['4','ACCTNAME_FORM_PRINCIPAL']],
    //                        name: 'authentication_Sql_accountCanonicalForm',
    //                        fieldLabel: this.app.i18n._('Account canonical form'),
    //                        value: '2'
    //                    }, {
    //                        name: 'authentication_Sql_accountDomainName',
    //                        fieldLabel: this.app.i18n._('Account domain name '),
    //                        inputType: 'text',
    //                        tabIndex: 7
    //                    }, {
    //                        name: 'authentication_Sql_accountDomainNameShort',
    //                        fieldLabel: this.app.i18n._('Account domain short name'),
    //                        inputType: 'text',
    //                        tabIndex: 8
    //                    } 
                        ]
                    }]
                }
            ]
        }, {
            xtype: 'fieldset',
            collapsible: false,
            autoHeight: true,
            title: this.app.i18n._('Accounts storage'),
            items: [
                this.accountsStorageCombo,
                {
                    id: this.accountsStorageIdPrefix + 'CardLayout',
                    layout: 'card',
                    activeItem: this.accountsStorageIdPrefix + 'Sql',
                    border: false,
                    defaults: {
                        border: false
                    },
                    items: [{
                        id: this.accountsStorageIdPrefix + 'Sql',
                        layout: 'form',
                        autoHeight: 'auto',
                        defaults: {
                            width: 300,
                            xtype: 'textfield',
                            tabIndex: this.getTabIndex
                        },
                        items: [{
                            name: 'accounts_Sql_defaultUserGroupName',
                            fieldLabel: this.app.i18n._('Default user group name')
                            //allowEmpty: false
                        }, {
                            name: 'accounts_Sql_defaultAdminGroupName',
                            fieldLabel: this.app.i18n._('Default admin group name')
                            //allowEmpty: false
                        }]
                    }, {
                        id: this.accountsStorageIdPrefix + 'Ldap',
                        layout: 'form',
                        autoHeight: 'auto',
                        defaults: {
                            width: 300,
                            xtype: 'textfield',
                            tabIndex: this.getTabIndex
                        },
                        items: [{
                            name: 'accounts_Ldap_host',
                            fieldLabel: this.app.i18n._('Host')
                        }, {
                            name: 'accounts_Ldap_username',
                            fieldLabel: this.app.i18n._('Login name')
                        }, {
                            name: 'accounts_Ldap_password',
                            fieldLabel: this.app.i18n._('Password'),
                            inputType: 'password'
                        }, 
                        Ext.applyIf({
                            name: 'accounts_Ldap_bindRequiresDn',
                            fieldLabel: this.app.i18n._('Bind requires DN'),
                            store: [['1', this.app.i18n._('Yes')], ['0', this.app.i18n._('No')]],
                            value: '1'
                        }, commonComboConfig), 
                        {
                            name: 'accounts_Ldap_userDn',
                            fieldLabel: this.app.i18n._('User DN')
                        }, {
                            name: 'accounts_Ldap_userFilter',
                            fieldLabel: this.app.i18n._('User Filter')
                        }, 
                        Ext.applyIf({
                            name: 'accounts_Ldap_userSearchScope',
                            fieldLabel: this.app.i18n._('User Search Scope'),
                            store: [['1', 'SEARCH_SCOPE_SUB'], ['2', 'SEARCH_SCOPE_ONE']],
                            value: '1'
                        }, commonComboConfig), 
                        {
                            name: 'accounts_Ldap_groupsDn',
                            fieldLabel: this.app.i18n._('Groups DN')
                        }, {
                            name: 'accounts_Ldap_groupFilter',
                            fieldLabel: this.app.i18n._('Group Filter')
                        }, 
                        Ext.applyIf({
                            name: 'accounts_Ldap_groupSearchScope',
                            fieldLabel: this.app.i18n._('Group Search Scope'),
                            store: [['1', 'SEARCH_SCOPE_SUB'], ['2', 'SEARCH_SCOPE_ONE']],
                            value: '1'
                        }, commonComboConfig), 
                        Ext.applyIf({
                            name: 'accounts_Ldap_pwEncType',
                            fieldLabel: this.app.i18n._('Password encoding'),
                            store: [['CRYPT', 'CRYPT'], ['SHA', 'SHA'], ['MD5', 'MD5']],
                            value: 'CRYPT'
                        }, commonComboConfig), 
                        Ext.applyIf({
                            name: 'accounts_Ldap_useRfc2307bis',
                            fieldLabel: this.app.i18n._('Use Rfc 2307 bis'),
                            store: [['0', this.app.i18n._('No')], ['1', this.app.i18n._('Yes')]],
                            value: '0'
                        }, commonComboConfig), 
                        {
                            name: 'accounts_Ldap_minUserId',
                            fieldLabel: this.app.i18n._('Min User Id')
                        }, {
                            name: 'accounts_Ldap_maxUserId',
                            fieldLabel: this.app.i18n._('Max User Id')
                        }, {
                            name: 'accounts_Ldap_minGroupId',
                            fieldLabel: this.app.i18n._('Min Group Id')
                        }, {
                            name: 'accounts_Ldap_maxGroupId',
                            fieldLabel: this.app.i18n._('Max Group Id')
                        }, {
                            name: 'accounts_Ldap_groupUUIDAttribute',
                            fieldLabel: this.app.i18n._('Group UUID Attribute name')
                        }, {
                            name: 'accounts_Ldap_userUUIDAttribute',
                            fieldLabel: this.app.i18n._('User UUID Attribute name')
                        }, {
                            name: 'accounts_Ldap_defaultUserGroupName',
                            fieldLabel: this.app.i18n._('Default user group name')
                        }, {
                            name: 'accounts_Ldap_defaultAdminGroupName',
                            fieldLabel: this.app.i18n._('Default admin group name')
                        },
                        Ext.applyIf({
                            name: 'accounts_Ldap_readonly',
                            fieldLabel: this.app.i18n._('Readonly access'),
                            store: [['0', this.app.i18n._('No')], ['1', this.app.i18n._('Yes')]],
                            value: '0'
                        }, commonComboConfig)]
                    }]
            } ]
          }, {
            xtype: 'fieldset',
            collapsible: false,
            autoHeight: true,
            title: this.app.i18n._('Password Settings'),
            defaults: {
                width: 300,
                xtype: 'uxspinner',
                tabIndex: this.getTabIndex,
                strategy: {
                    xtype: 'number',
                    minValue: 0,
                    maxValue: 64
                },
                value: 0
            },
            items: [
            Ext.applyIf({
                name: 'password_changepw',
                fieldLabel: this.app.i18n._('User can change password'),
                store: [[1, this.app.i18n._('Yes')], [0, this.app.i18n._('No')]],
                value: 1
            }, commonComboConfig),
            Ext.applyIf({
                name: 'password_pwPolicyActive',
                fieldLabel: this.app.i18n._('Enable password policy'),
                store: [[1, this.app.i18n._('Yes')], [0, this.app.i18n._('No')]],
                value: 0
            }, commonComboConfig),
            Ext.applyIf({
                name: 'password_pwPolicyOnlyASCII',
                fieldLabel: this.app.i18n._('Only ASCII'),
                store: [[1, this.app.i18n._('Yes')], [0, this.app.i18n._('No')]],
                value: 0
            }, commonComboConfig), {
                name: 'password_pwPolicyMinLength',
                fieldLabel: this.app.i18n._('Minimum length')
            }, {
                name: 'password_pwPolicyMinWordChars',
                fieldLabel: this.app.i18n._('Minimum word chars')
            }, {
                name: 'password_pwPolicyMinUppercaseChars',
                fieldLabel: this.app.i18n._('Minimum uppercase chars')
            }, {
                name: 'password_pwPolicyMinSpecialChars',
                fieldLabel: this.app.i18n._('Minimum special chars')
            }, {
                name: 'password_pwPolicyMinNumbers',
                fieldLabel: this.app.i18n._('Minimum numbers')
            }
            ]
        }, {
            xtype: 'fieldset',
            collapsible: false,
            autoHeight: true,
            title: this.app.i18n._('Redirect Settings'),
            defaults: {
                width: 300,
                xtype: 'textfield',
                tabIndex: this.getTabIndex
            },
            items: [{
                name: 'redirectSettings_redirectUrl',
                fieldLabel: this.app.i18n._('Redirect Url (redirect to login screen if empty)')
            }, 
            Ext.applyIf({
                name: 'redirectSettings_redirectAlways',
                fieldLabel: this.app.i18n._('Redirect Always (if No, only redirect after logout)'),
                store: [['1', this.app.i18n._('Yes')], ['0', this.app.i18n._('No')]],
                value: '0'
            }, commonComboConfig), 
            Ext.applyIf({
                name: 'redirectSettings_redirectToReferrer',
                fieldLabel: this.app.i18n._('Redirect to referring site, if exists'),
                store: [['1', this.app.i18n._('Yes')], ['0', this.app.i18n._('No')]],
                value: '0'
            }, commonComboConfig)]
        }];
    },
    
    /**
     * applies registry state to this cmp
     */
    applyRegistryState: function () {
        this.action_saveConfig.setDisabled(false);
        
        if (Tine.Setup.registry.get('setupRequired')) {
            this.action_saveConfig.setText(this.app.i18n._('Save config and install'));
        } else {
            this.action_saveConfig.setText(this.app.i18n._('Save config'));
            this.getForm().findField('authentication_Sql_adminPassword').setDisabled(true);
            this.getForm().findField('authentication_Sql_adminPasswordConfirmation').setDisabled(true);
            this.getForm().findField('authentication_Sql_adminLoginName').setDisabled(true);
        }
    },
    
    /**
     * checks if form is valid
     * - password fields are equal
     * 
     * @return {Boolean}
     */
    isValid: function () {
        var form = this.getForm();

        var result = form.isValid();
        
        // check if passwords match
        if (this.authenticationBackendCombo.getValue() === 'Sql' && 
            form.findField('authentication_Sql_adminPassword') && 
            form.findField('authentication_Sql_adminPassword').getValue() !== form.findField('authentication_Sql_adminPasswordConfirmation').getValue()) 
        {
            form.markInvalid([{
                id: 'authentication_Sql_adminPasswordConfirmation',
                msg: this.app.i18n._("Passwords don't match")
            }]);
            result = false;
        }
        
        // check if initial username/passwords are set
        if (Tine.Setup.registry.get('setupRequired') && form.findField('authentication_Sql_adminLoginName')) {
            if (Ext.isEmpty(form.findField('authentication_Sql_adminLoginName').getValue())) {
                form.markInvalid([{
                    id: 'authentication_Sql_adminLoginName',
                    msg: this.app.i18n._("Should not be empty")
                }]);
                result = false;
            }
            if (Ext.isEmpty(form.findField('authentication_Sql_adminPassword').getValue())) {
                form.markInvalid([{
                    id: 'authentication_Sql_adminPassword',
                    msg: this.app.i18n._("Should not be empty")
                }]);
                form.markInvalid([{
                    id: 'authentication_Sql_adminPasswordConfirmation',
                    msg: this.app.i18n._("Should not be empty")
                }]);
                result = false;
            }
        }
        
        if (this.accountsStorageCombo.getValue() === 'Sql' && 
               form.findField('accounts_Sql_defaultUserGroupName') && 
               Ext.isEmpty(form.findField('accounts_Sql_defaultUserGroupName').getValue())) 
          {
            form.markInvalid([{
                id: 'accounts_Sql_defaultUserGroupName',
                msg: this.app.i18n._("Should not be empty")
            }]);
            result = false;
        }
        
        if (this.accountsStorageCombo.getValue() === 'Sql' && 
            form.findField('accounts_Sql_defaultAdminGroupName') && 
            Ext.isEmpty(form.findField('accounts_Sql_defaultAdminGroupName').getValue())) 
        {
            form.markInvalid([{
                id: 'accounts_Sql_defaultAdminGroupName',
                msg: this.app.i18n._("Should not be empty")
            }]);
            result = false;
        }
        
        return result;
    }
});/*
 * Tine 2.0
 * 
 * @package     Setup
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2009-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */

/*global Ext, Tine*/

Ext.ns('Tine', 'Tine.Setup');
 
/**
 * Setup Email Config Manager
 * 
 * @namespace   Tine.Setup
 * @class       Tine.Setup.EmailPanel
 * @extends     Tine.Tinebase.widgets.form.ConfigPanel
 * 
 * <p>Email Config Panel</p>
 * <p><pre>
 * </pre></p>
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schuele <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2009 Metaways Infosystems GmbH (http://www.metaways.de)
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Setup.EmailPanel
 */
Tine.Setup.EmailPanel = Ext.extend(Tine.Tinebase.widgets.form.ConfigPanel, {
    
    /**
     * @property idPrefix DOM Id prefix
     * @type String
     */
    idPrefix: null,
    
    /**
     * imapBackend DOM Id prefix
     * 
     * @property imapBackendIdPrefix
     * @type String
     */
    imapBackendIdPrefix: null,
    
    /**
     * combo box containing the imap backend selection
     * 
     * @property imapBackendCombo
     * @type Ext.form.ComboBox 
     */
    imapBackendCombo: null,

    /**
     * smtpBackend DOM Id prefix
     * 
     * @property smtpBackendIdPrefix
     * @type String
     */
    smtpBackendIdPrefix: null,
    
    /**
     * combo box containing the smtp backend selection
     * 
     * @property smtpBackendCombo
     * @type Ext.form.ComboBox 
     */
    smtpBackendCombo: null,

    /**
     * show type before db settings
     * @cfg {Boolean} showType
     */
    showType: false,
    
    /**
     * @private
     * panel cfg
     */
    saveMethod: 'Setup.saveEmailConfig',
    registryKey: 'emailData',
    
    defaults: {
        xtype: 'fieldset',
        autoHeight: 'auto',
        defaults: {width: 300}
    },
    
    /**
     * @private
     */
    initComponent: function () {
        this.idPrefix                  = Ext.id();
        this.imapBackendIdPrefix       = this.idPrefix + '-imapBackend-';
        this.smtpBackendIdPrefix       = this.idPrefix + '-smtpBackend-';

        Tine.Setup.EmailPanel.superclass.initComponent.call(this);
    },
    
    /**
     * Change IMAP card layout depending on selected combo box entry
     */
    onChangeImapBackend: function () {
        this.changeCard(this.imapBackendCombo, this.imapBackendIdPrefix);
    },

    /**
     * Change SMTP card layout depending on selected combo box entry
     */
    onChangeSmtpBackend: function () {
        this.changeCard(this.smtpBackendCombo, this.smtpBackendIdPrefix);
    },

    /**
     * @private
     */
    onRender: function (ct, position) {
        Tine.Setup.EmailPanel.superclass.onRender.call(this, ct, position);
        
        this.onChangeImapBackend.defer(250, this);
        this.onChangeSmtpBackend.defer(250, this);
    },
    
   /**
     * returns config manager form
     * 
     * @private
     * @return {Array} items
     */
    getFormItems: function () {
        
        var backendComboConfig = {
            xtype: 'combo',
            width: 300,
            listWidth: 300,
            mode: 'local',
            forceSelection: true,
            allowEmpty: false,
            triggerAction: 'all',
            editable: false,
            value: 'standard',
            fieldLabel: this.app.i18n._('Backend')
        };
        
        // imap combo
        backendComboConfig.store = [['standard', this.app.i18n._('Standard IMAP')], ['dbmail', 'DBmail  MySQL'], ['ldap_imap', 'DBmail Ldap'], ['cyrus', 'Cyrus'], ['dovecot_imap', 'Dovecot MySQL']];
        backendComboConfig.name = 'imap_backend';
        backendComboConfig.listeners = {
            scope: this,
            change: this.onChangeImapBackend,
            select: this.onChangeImapBackend
        };
        this.imapBackendCombo = new Ext.form.ComboBox(backendComboConfig);
        
        // smtp combo
        backendComboConfig.store = [['standard', this.app.i18n._('Standard SMTP')], ['postfix', 'Postfix MySQL'], ['Ldapsmtpmail', 'Ldap (only mail attribute)'], ['ldapSmtp', 'Postfix Ldap (dbmail schema)'], ['ldapSmtpQmail', 'Postfix Ldap (qmail schema)']];
        backendComboConfig.name = 'smtp_backend';
        backendComboConfig.listeners = {
            scope: this,
            change: this.onChangeSmtpBackend,
            select: this.onChangeSmtpBackend
        };
        this.smtpBackendCombo = new Ext.form.ComboBox(backendComboConfig);

        // use backend combo config as common combo config
        var commonComboConfig = backendComboConfig;
        delete commonComboConfig.listeners;
        
        return [{
            title: this.app.i18n._('Imap'),
            id: 'setup-imap-group',
            checkboxToggle: true,
            collapsed: true,
            items: [
                this.imapBackendCombo, 
                {
                    name: 'imap_host',
                    fieldLabel: this.app.i18n._('Hostname'),
                    xtype: 'textfield'
                }, /*{
                    name: 'imap_user',
                    fieldLabel: this.app.i18n._('Username'),
                    xtype: 'textfield'
                }, {
                    name: 'imap_password',
                    fieldLabel: this.app.i18n._('Password'),
                    xtype: 'textfield',
                    inputType: 'password'
                }, */{
                    name: 'imap_port',
                    fieldLabel: this.app.i18n._('Port'),
                    xtype: 'numberfield'
                }, 
                Ext.applyIf({
                    fieldLabel: this.app.i18n._('Secure Connection'),
                    name: 'imap_ssl',
                    value: 'none',
                    store: [['none', this.app.i18n._('None')], ['tls',  this.app.i18n._('TLS')], ['ssl',  this.app.i18n._('SSL')]]
                }, commonComboConfig), 
                Ext.applyIf({
                    name: 'imap_useSystemAccount',
                    fieldLabel: this.app.i18n._('Use system account'),
                    store: [[0, this.app.i18n._('No')], [1, this.app.i18n._('Yes')]],
                    value: 0
                }, commonComboConfig), 
                {
                    name: 'imap_domain',
                    fieldLabel: this.app.i18n._('Append domain to login name'),
                    xtype: 'textfield'
                }, {
                    id: this.imapBackendIdPrefix + 'CardLayout',
                    layout: 'card',
                    activeItem: this.imapBackendIdPrefix + 'standard',
                    border: false,
                    width: '100%',
                    defaults: {
                        border: false
                    },
                    items: [{
                        // nothing in here yet
                        id: this.imapBackendIdPrefix + 'standard',
                        layout: 'form',
                        items: []
                    }, {
                        // dbmail config options
                        id: this.imapBackendIdPrefix + 'dbmail',
                        layout: 'form',
                        autoHeight: 'auto',
                        defaults: {
                            width: 300,
                            xtype: 'textfield'
                        },
                        items: this.getDbConfigFields('imap', 'dbmail')
                    }, {
                        // nothing in here yet
                        id: this.imapBackendIdPrefix + 'ldap_imap',
                        layout: 'form',
                        defaults: {
                            width: 300,
                            xtype: 'textfield'
                        },
                        items: []
                    }, {
                        // cyrus config options
                        id: this.imapBackendIdPrefix + 'cyrus',
                        layout: 'form',
                        autoHeight: 'auto',
                        defaults: {
                            width: 300,
                            xtype: 'textfield'
                        },
                        items: [{
                            name: 'imap_cyrus_admin',
                            fieldLabel: this.app.i18n._('Cyrus Admin')
                        }, {
                            name: 'imap_cyrus_password',
                            fieldLabel: this.app.i18n._('Cyrus Admin Password'),
                            inputType: 'password'
                        }]
                    }, {
                        // dovecot config options
                        id: this.imapBackendIdPrefix + 'dovecot_imap',
                        layout: 'form',
                        autoHeight: 'auto',
                        defaults: {
                            width: 300,
                            xtype: 'textfield'
                        },
                        items: this.getDbConfigFields('imap', 'dovecot').concat(this.getDovecotExtraConfig('imap'))
                    }]
                }
            ]
        }, {
            title: this.app.i18n._('Smtp'),
            id: 'setup-smtp-group',
            checkboxToggle:true,
            collapsed: true,
            items: [
                this.smtpBackendCombo,
                {
                    name: 'smtp_hostname',
                    fieldLabel: this.app.i18n._('Hostname'),
                    xtype: 'textfield'
                }, {
                    name: 'smtp_port',
                    fieldLabel: this.app.i18n._('Port'),
                    xtype: 'numberfield'
                }, 
                Ext.applyIf({
                    fieldLabel: this.app.i18n._('Secure Connection'),
                    name: 'smtp_ssl',
                    value: 'none',
                    store: [['none', this.app.i18n._('None')], ['tls', this.app.i18n._('TLS')], ['ssl', this.app.i18n._('SSL')]]
                }, commonComboConfig), 
                Ext.applyIf({
                    fieldLabel: this.app.i18n._('Authentication'),
                    name: 'smtp_auth',
                    value: 'login',
                    store: [['none', this.app.i18n._('None')], ['login', this.app.i18n._('Login')], ['plain', this.app.i18n._('Plain')]]
                }, commonComboConfig), 
                {
                    name: 'smtp_primarydomain',
                    fieldLabel: this.app.i18n._('Primary Domain'),
                    xtype: 'textfield'
                }, {
                    name: 'smtp_secondarydomains',
                    fieldLabel: this.app.i18n._('Secondary Domains (comma separated)'),
                    xtype: 'textfield'
                }, {
                    name: 'smtp_from',
                    fieldLabel: this.app.i18n._('Notifications service address'),
                    xtype: 'textfield'
                }, {
                    name: 'smtp_username',
                    fieldLabel: this.app.i18n._('Notification Username'),
                    xtype: 'textfield'
                }, {
                    name: 'smtp_password',
                    fieldLabel: this.app.i18n._('Notification Password'),
                    inputType: 'password',
                    xtype: 'textfield'
                }, {
                    name: 'smtp_name',
                    fieldLabel: this.app.i18n._('Notifications local client (hostname or IP address)'),
                    xtype: 'textfield',
                    value: 'localhost'
                }, {
                    id: this.smtpBackendIdPrefix + 'CardLayout',
                    layout: 'card',
                    activeItem: this.smtpBackendIdPrefix + 'standard',
                    border: false,
                    width: '100%',
                    defaults: {
                        border: false
                    },
                    items: [{
                        // nothing in here yet
                        id: this.smtpBackendIdPrefix + 'standard',
                        layout: 'form',
                        items: []
                    }, {
                        // nothing in here yet
                        id: this.smtpBackendIdPrefix + 'ldap_smtp_mail',
                        layout: 'form',
                        items: []
                    },{
                        // postfix config options
                        id: this.smtpBackendIdPrefix + 'postfix',
                        layout: 'form',
                        autoHeight: 'auto',
                        defaults: {
                            width: 300,
                            xtype: 'textfield'
                        },
                        items: this.getDbConfigFields('smtp', 'postfix')
                    }, {
                        // postfix config options
                        id: this.smtpBackendIdPrefix + 'ldap_smtp',
                        layout: 'form',
                        autoHeight: 'auto',
                        defaults: {
                            width: 300,
                            xtype: 'textfield'
                        },
                        items: []
                    }, {
                        // postfix ldap qmail user schema config options
                        id: this.smtpBackendIdPrefix + 'ldap_smtp_qmail',
                        layout: 'form',
                        autoHeight: 'auto',
                        defaults: {
                            width: 300,
                            xtype: 'textfield'
                        },
                        items: []
                    }]
                }
             ]
        }, {
            title: this.app.i18n._('SIEVE'),
            id: 'setup-sieve-group',
            checkboxToggle:true,
            collapsed: true,
            items: [{
                name: 'sieve_hostname',
                fieldLabel: this.app.i18n._('Hostname'),
                xtype: 'textfield'
            }, {
                name: 'sieve_port',
                fieldLabel: this.app.i18n._('Port'),
                xtype: 'numberfield'
            }, 
            Ext.applyIf({
                fieldLabel: this.app.i18n._('Secure Connection'),
                name: 'sieve_ssl',
                value: 'none',
                store: [['none', this.app.i18n._('None')], ['tls', this.app.i18n._('TLS')], ['ssl', this.app.i18n._('SSL')]]
            }, commonComboConfig)]
        }];
    },
    
    /**
     * applies registry state to this cmp
     */
    applyRegistryState: function () {
        this.action_saveConfig.setDisabled(!this.isValid());
    },
    
    /**
     * get db config fields
     * 
     * @param {String} type1 (imap, smtp)
     * @param {String} type2 (dbmail, postfix, ...)
     * @return {Array}
     */
    getDbConfigFields: function (type1, type2) {
        var typeString = (this.showType) ? (Ext.util.Format.capitalize(type2) + ' ') : '';
        
        return [{
            name: type1 + '_' + type2 + '_host',
            fieldLabel: typeString + this.app.i18n._('MySql Hostname')
        }, {
            name: type1 + '_' + type2 + '_dbname',
            fieldLabel: typeString + this.app.i18n._('MySql Database')
        }, {
            name: type1 + '_' + type2 + '_username',
            fieldLabel: typeString + this.app.i18n._('MySql User')
        }, {
            name: type1 + '_' + type2 + '_password',
            fieldLabel: typeString + this.app.i18n._('MySql Password'),
            inputType: 'password'
        }, {
            name: type1 + '_' + type2 + '_port',
            fieldLabel: typeString + this.app.i18n._('MySql Port'),
            value: 3306
        }];
    },
    
    /**
     * get db config fields
     * 
     * @param {String} type1 (imap, smtp)
     * @return {Array}
     */
    getDovecotExtraConfig: function (type1) {
        var typeString = (this.showType) ? 'Dovecot ' : '';
        
        return [{
            name: type1 + '_dovecot_uid',
            fieldLabel: typeString + this.app.i18n._('User or UID')
        }, {
            name: type1 + '_dovecot_gid',
            fieldLabel: typeString + this.app.i18n._('Group or GID')
        }, {
            name: type1 + '_dovecot_home',
            fieldLabel: typeString + this.app.i18n._('Home Template')
        }, {
            fieldLabel: typeString + this.app.i18n._('Password Scheme'),
            name: type1 + '_dovecot_scheme',
            typeAhead     : false,
            triggerAction : 'all',
            lazyRender    : true,
            editable      : false,
            mode          : 'local',
            xtype: 'combo',
            listWidth: 300,
            value: 'PLAIN-MD5',
            store: [
                ['PLAIN-MD5',      this.app.i18n._('PLAIN-MD5')],
                ['MD5-CRYPT',    this.app.i18n._('MD5-CRYPT')],
                ['SHA',           this.app.i18n._('SHA1')],
                ['SHA256',       this.app.i18n._('SHA256')],
                ['SSHA256',      this.app.i18n._('SSHA256')],
                ['SHA512',       this.app.i18n._('SHA512')],
                ['SSHA512',       this.app.i18n._('SSHA512')],
                ['PLAIN',        this.app.i18n._('PLAIN')]
            ]
        }];
    }
});
