/*!
 * Tine 2.0 - Projects 
 * Copyright (c) 2011 Metaways Infosystems GmbH (http://www.metaways.de)http://www.gnu.org/licenses/agpl.html AGPL Version 3
 */
/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2011 Metaways Infosystems GmbH (http://www.metaways.de)
 */
Ext.ns('Tine.Projects');

/**
 * Foreign Record Filter
 * 
 * @namespace   Tine.Projects
 * @class       Tine.Projects.ProjectAttendeeFilter
 * @extends     Tine.widgets.grid.ForeignRecordFilter
 * 
 * <p>Filter for project attendee</p>
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * 
 * @param       {Object} config
 * @constructor
 */
Tine.Projects.ProjectAttendeeFilter = Ext.extend(Tine.widgets.grid.ForeignRecordFilter, {

    /**
     * @cfg {Record} foreignRecordClass (required)
     */
    foreignRecordClass: Tine.Addressbook.Model.Contact,
    
    /**
     * @cfg {String} ownField (required)
     */
    ownField: 'contact',
    
    initComponent: function() {
        this.label = this.app.i18n._('Attendee');
        
        Tine.Projects.ProjectAttendeeFilter.superclass.initComponent.call(this);
    },
    
    /**
     * get subfilter models
     * @return Array of filter models
     */
    getSubFilters: function() {
        var attendeeRoleFilter = new Tine.Tinebase.widgets.keyfield.Filter({
            label: this.app.i18n._('Attendee Role'),
            field: 'relation_type',
            app: this.app, 
            keyfieldName: 'projectAttendeeRole'
        });
        
        return [attendeeRoleFilter];
    }
});

Tine.widgets.grid.FilterToolbar.FILTERS['tine.projects.attendee'] = Tine.Projects.ProjectAttendeeFilter;
/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2011 Metaways Infosystems GmbH (http://www.metaways.de)
 */
Ext.ns('Tine.Projects.Model');

/**
 * @namespace   Tine.Projects.Model
 * @class       Tine.Projects.Model.Project
 * @extends     Tine.Tinebase.data.Record
 * Example record definition
 * 
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 */
Tine.Projects.Model.Project = Tine.Tinebase.data.Record.create(Tine.Tinebase.Model.genericFields.concat([
    { name: 'id' },
    { name: 'title' },
    { name: 'number' },
    { name: 'description' },
    { name: 'status' },
    { name: 'relations' },
    // tine 2.0 notes + tags
    { name: 'notes'},
    { name: 'tags' }
]), {
    appName: 'Projects',
    modelName: 'Project',
    idProperty: 'id',
    titleProperty: 'title',
    // ngettext('Project', 'Projects', n);
    recordName: 'Project',
    recordsName: 'Projects',
    containerProperty: 'container_id',
    // ngettext('Project list', 'Project lists', n);
    containerName: 'Project list',
    containersName: 'Project lists'
    // _('Project lists')
});

/**
 * @namespace Tine.Projects.Model
 * 
 * get default data for a new record
 *  
 * @return {Object} default data
 * @static
 */ 
Tine.Projects.Model.Project.getDefaultData = function() {
    var app = Tine.Tinebase.appMgr.get('Projects');
    var defaultsContainer = Tine.Projects.registry.get('defaultContainer');
    
    return {
        container_id: app.getMainScreen().getWestPanel().getContainerTreePanel().getDefaultContainer(),
        status: 'IN-PROCESS'
    };
};

/**
 * get filtermodel of projects
 * 
 * @namespace Tine.ExampleApplication.Model
 * @static
 * @return {Object} filterModel definition
 */ 
Tine.Projects.Model.Project.getFilterModel = function() {
    var app = Tine.Tinebase.appMgr.get('Projects');
    
    return [ 
        {label: _('Quick search'),    field: 'query',       operators: ['contains']},
        {label: app.i18n._('Title'),    field: 'title'},
        {label: app.i18n._('Number'),    field: 'number'},
        {label: app.i18n._('Description'),    field: 'description'},
        {
            label: app.i18n._('Status'),
            field: 'status',
            filtertype: 'tine.widget.keyfield.filter', 
            app: app, 
            keyfieldName: 'projectStatus'
        },
        {filtertype: 'tinebase.tag', app: app},
        {filtertype: 'tine.widget.container.filtermodel', app: app, recordClass: Tine.Projects.Model.Project},
        {filtertype: 'tine.projects.attendee', app: app},
        {label: _('Last Modified Time'),                                                field: 'last_modified_time', valueType: 'date'},
        {label: _('Last Modified By'),                                                  field: 'last_modified_by',   valueType: 'user'},
        {label: _('Creation Time'),                                                     field: 'creation_time',      valueType: 'date'},
        {label: _('Created By'),                                                        field: 'created_by',         valueType: 'user'}
    ];
};

/**
 * default Project backend
 */
Tine.Projects.recordBackend = new Tine.Tinebase.data.RecordProxy({
    appName: 'Projects',
    modelName: 'Project',
    recordClass: Tine.Projects.Model.Project
});
/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2011 Metaways Infosystems GmbH (http://www.metaways.de)
 */
Ext.ns('Tine.Projects');

/**
 * @namespace   Tine.Projects
 * @class       Tine.Projects.Application
 * @extends     Tine.Tinebase.Application
 * 
 * @author      Philipp Schüle <p.schuele@metaways.de>
 */
Tine.Projects.Application = Ext.extend(Tine.Tinebase.Application, {
    /**
     * Get translated application title of the calendar application
     * 
     * @return {String}
     */
    getTitle: function() {
        return this.i18n.gettext('Projects');
    },
    
    init: function() {
        new Tine.Projects.AddressbookGridPanelHook({app: this});

    }
});

/**
 * @namespace   Tine.Projects
 * @class       Tine.Projects.MainScreen
 * @extends     Tine.widgets.MainScreen
 * 
 * @author      Philipp Schüle <p.schuele@metaways.de>
 */
Tine.Projects.MainScreen = Ext.extend(Tine.widgets.MainScreen, {
    activeContentType: 'Project'
});

/**
 * @namespace   Tine.Projects
 * @class       Tine.Projects.TreePanel
 * @extends     Tine.widgets.container.TreePanel
 * 
 * @author      Philipp Schüle <p.schuele@metaways.de>
 */
Tine.Projects.ProjectTreePanel = Ext.extend(Tine.widgets.container.TreePanel, {
    id: 'Projects_Tree',
    filterMode: 'filterToolbar',
    recordClass: Tine.Projects.Model.Project
});

/**
 * @class       Tine.Projects.FilterPanel
 * @extends     Tine.widgets.persistentfilter.PickerPanel
 *  
 * @param {Object} config
 */
Tine.Projects.ProjectFilterPanel = function(config) {
    Ext.apply(this, config);
    Tine.Projects.ProjectFilterPanel.superclass.constructor.call(this);
};

Ext.extend(Tine.Projects.ProjectFilterPanel, Tine.widgets.persistentfilter.PickerPanel, {
    filter: [{field: 'model', operator: 'equals', value: 'Projects_Model_ProjectFilter'}]
});
/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2011 Metaways Infosystems GmbH (http://www.metaways.de)
 */
Ext.ns('Tine.Projects');

/**
 * Project grid panel
 * 
 * @namespace   Tine.Projects
 * @class       Tine.Projects.ProjectGridPanel
 * @extends     Tine.widgets.grid.GridPanel
 * 
 * <p>Project Grid Panel</p>
 * <p><pre>
 * </pre></p>
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schuele <p.schuele@metaways.de>
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Projects.ProjectGridPanel
 */
Tine.Projects.ProjectGridPanel = Ext.extend(Tine.widgets.grid.GridPanel, {
    /**
     * record class
     * @cfg {Tine.Projects.Model.Project} recordClass
     */
    recordClass: Tine.Projects.Model.Project,
    
    /**
     * eval grants
     * @cfg {Boolean} evalGrants
     */
    evalGrants: true,
    
    /**
     * optional additional filterToolbar configs
     * @cfg {Object} ftbConfig
     */
    ftbConfig: null,
    
    /**
     * grid specific
     * @private
     */
    defaultSortInfo: {field: 'creation_time', direction: 'DESC'},
    gridConfig: {
        autoExpandColumn: 'title'
    },
     
    /**
     * inits this cmp
     * @private
     */
    initComponent: function() {
        this.recordProxy = Tine.Projects.recordBackend;
        
        this.gridConfig.cm = this.getColumnModel();
        this.filterToolbar = this.filterToolbar || this.getFilterToolbar(this.ftbConfig);
        
        this.plugins = this.plugins || [];
        this.plugins.push(this.filterToolbar);
        
        Tine.Projects.ProjectGridPanel.superclass.initComponent.call(this);
    },
    
    /**
     * returns cm
     * 
     * @return Ext.grid.ColumnModel
     * @private
     */
    getColumnModel: function(){
        return new Ext.grid.ColumnModel({
            defaults: {
                sortable: true,
                resizable: true
            },
            columns: [
            {   id: 'tags', header: this.app.i18n._('Tags'), width: 40,  dataIndex: 'tags', sortable: false, renderer: Tine.Tinebase.common.tagsRenderer },                
            {
                id: 'number',
                header: this.app.i18n._("Number"),
                width: 100,
                sortable: true,
                dataIndex: 'number',
                hidden: true
            }, {
                id: 'title',
                header: this.app.i18n._("Title"),
                width: 350,
                sortable: true,
                dataIndex: 'title'
            }, {
                id: 'status',
                header: this.app.i18n._("Status"),
                width: 150,
                sortable: true,
                dataIndex: 'status',
                renderer: Tine.Tinebase.widgets.keyfield.Renderer.get('Projects', 'projectStatus')
            }].concat(this.getModlogColumns())
        });
    },
    
    /**
     * status column renderer
     * @param {string} value
     * @return {string}
     */
    statusRenderer: function(value) {
        return this.app.i18n._hidden(value);
    }
});
/*
 * Tine 2.0
 * 
 * @package     Projects
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Alexander Stintzing <alex@stintzing.net>
 * @copyright   Copyright (c) 2009-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.ns('Tine.Projects');

/**
 * @namespace   Tine.Projects
 * @class       Tine.Projects.AddressbookGridPanelHook
 * 
 * <p>Projects Addressbook Hook</p>
 * <p>
 * </p>
 * 
 * @author      Alexander Stintzing <alex@stintzing.net>
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * 
 * @constructor
 */
Tine.Projects.AddressbookGridPanelHook = function(config) {
    Tine.log.info('initialising projects addressbook hooks');
    Ext.apply(this, config);

    var text = this.app.i18n.n_hidden(Tine.Projects.Model.Project.prototype.recordName, Tine.Projects.Model.Project.prototype.recordsName, 1);
    
    this.addProjectAction = new Ext.Action({
        requiredGrant: 'readGrant',
        text: text,
        iconCls: this.app.getIconCls(),
        scope: this,
        handler: this.onUpdateProject,
        listeners: {
            scope: this,
            render: this.onRender
        }
    });
    
    this.newProjectAction = new Ext.Action({
        requiredGrant: 'readGrant',
        text: text,
        iconCls: this.app.getIconCls(),
        scope: this,
        handler: this.onAddProject,
        listeners: {
            scope: this,
            render: this.onRender
        }
    });
    
    Ext.ux.ItemRegistry.registerItem('Addressbook-GridPanel-ContextMenu-Add', this.addProjectAction, 90);
    Ext.ux.ItemRegistry.registerItem('Addressbook-GridPanel-ContextMenu-New', this.newProjectAction, 90);
};

Ext.apply(Tine.Projects.AddressbookGridPanelHook.prototype, {
    
    /**
     * @property app
     * @type Tine.Projects.Application
     * @private
     */
    app: null,
    
    /**
     * @property handleProjectsAction
     * @type Tine.widgets.ActionUpdater
     * @private
     */
    handleProjectsAction: null,
    
    /**
     * @property ContactGridPanel
     * @type Tine.Addressbook.ContactGridPanel
     * @private
     */
    ContactGridPanel: null,
    
    projectsMenu: null,
    
    /**
     * get addressbook contact grid panel
     */
    getContactGridPanel: function() {
        if (! this.ContactGridPanel) {
            this.ContactGridPanel = Tine.Tinebase.appMgr.get('Addressbook').getMainScreen().getCenterPanel();
        }
        return this.ContactGridPanel;
    },

    /**
     * adds contacts to a new project
     */
    onAddProject: function() {
        var ms = this.app.getMainScreen(),
            cp = ms.getCenterPanel(),
            filter = this.getFilter(ms),
            sm = this.getContactGridPanel().selectionModel;
            
        if(!filter) {
            var addRelations = this.getSelectionsAsArray();
        } else {
            var addRelations = true;
        }
        
        cp.onEditInNewWindow.call(cp, 'add', null, [{
            ptype: 'addrelations_edit_dialog', selectionFilter: filter, addRelations: addRelations, callingApp: 'Addressbook', callingModel: 'Contact'
        }]);
    },
    
    /**
     * adds contacts to an existing project
     */
    onUpdateProject: function() {
        var ms = this.app.getMainScreen(),
            cp = ms.getCenterPanel(),
            filter = this.getFilter(ms),
            sm = this.getContactGridPanel().selectionModel;
        
        if(!filter) {
            var addRelations = this.getSelectionsAsArray(),
                count = this.getSelectionsAsArray().length;
        } else {
            var addRelations = true,
                count = sm.store.totalLength;
        }

        Tine.Projects.AddToProjectPanel.openWindow(
            {count: count, selectionFilter: filter, addRelations: addRelations, callingApp: 'Addressbook', callingModel: 'Contact'}
        );
    },
    
    /**
     * returns the current filter if is filter selection
     * @param {Tine.widgets.MainScreen}
     * @return {Object}
     */
    getFilter: function(ms) {
        var sm = this.getContactGridPanel().selectionModel,
            addRelations = this.getSelectionsAsArray(),
            filter = null;
            
        if(sm.isFilterSelect) {
            var filter = sm.getSelectionFilter();
        }
        return filter;
    },
    
    /**
     * gets the current selection as an array 
     */
    getSelectionsAsArray: function() {
        var contacts = this.getContactGridPanel().grid.getSelectionModel().getSelections(),
            cont = [];
            
        Ext.each(contacts, function(contact) {
           if(contact.data) cont.push(contact.data);
        });
        
        return cont;
    },
    
    /**
     * add to action updater the first time we render
     */
    onRender: function() {
        var actionUpdater = this.getContactGridPanel().actionUpdater,
            registeredActions = actionUpdater.actions;
            
        if (registeredActions.indexOf(this.addEventAction) < 0) {
            actionUpdater.addActions([this.addEventAction]);
        }
        
        if (registeredActions.indexOf(this.newEventAction) < 0) {
            actionUpdater.addActions([this.newEventAction]);
        }        
    }
});
/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2011 Metaways Infosystems GmbH (http://www.metaways.de)
 */
Ext.ns('Tine.Projects');

/**
 * @namespace   Tine.Projects
 * @class       Tine.Projects.ProjectEditDialog
 * @extends     Tine.widgets.dialog.EditDialog
 * 
 * <p>Project Compose Dialog</p>
 * <p></p>
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Projects.ProjectEditDialog
 */
Tine.Projects.ProjectEditDialog = Ext.extend(Tine.widgets.dialog.EditDialog, {
    
    /**
     * @private
     */
    windowNamePrefix: 'ProjectEditWindow_',
    appName: 'Projects',
    recordClass: Tine.Projects.Model.Project,
    recordProxy: Tine.Projects.recordBackend,
    tbarItems: [{xtype: 'widget-activitiesaddbutton'}],
    evalGrants: true,
    showContainerSelector: true,
    hideRelationsPanel: true,
    
    /**
     * overwrite update toolbars function (we don't have record grants yet)
     * @private
     */
    updateToolbars: function() {

    },
    
    /**
     * executed after record got updated from proxy
     * @private
     */
    onAfterRecordLoad: function() {
        Tine.Projects.ProjectEditDialog.superclass.onAfterRecordLoad.call(this);
        this.contactLinkPanel.onRecordLoad(this.record);
    },
    
    /**
     * executed when record gets updated from form
     * - add attachments to record here
     * 
     * @private
     */
    onRecordUpdate: function() {
        Tine.Projects.ProjectEditDialog.superclass.onRecordUpdate.call(this);
        this.record.set('relations', this.contactLinkPanel.getData());
    },
    
    /**
     * returns dialog
     * 
     * NOTE: when this method gets called, all initalisation is done.
     * 
     * @return {Object}
     * @private
     */
    getFormItems: function() {
        this.contactLinkPanel = new Tine.widgets.grid.LinkGridPanel({
            app: this.app,
            searchRecordClass: Tine.Addressbook.Model.Contact,
            title: this.app.i18n._('Attendee'),
            typeColumnHeader: this.app.i18n._('Role'),
            searchComboClass: Tine.Addressbook.SearchCombo,
            searchComboConfig: {
                relationDefaults: {
                    type: this.app.getRegistry().get('config')['projectAttendeeRole'].definition['default'],
                    own_model: 'Projects_Model_Project',
                    related_model: 'Addressbook_Model_Contact',
                    own_degree: 'sibling',
                    related_backend: 'Sql'
                }
            },
            relationTypesKeyfieldName: 'projectAttendeeRole'
        });
        
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
                title: this.app.i18n._('Project'),
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
                        title: this.app.i18n._('Project'),
                        items: [{
                            xtype: 'columnform',
                            labelAlign: 'top',
                            formDefaults: {
                                xtype:'textfield',
                                anchor: '100%',
                                labelSeparator: '',
                                columnWidth: .333
                            },
                            items: [[{
                                    columnWidth: 1,
                                    fieldLabel: this.app.i18n._('Title'),
                                    name: 'title',
                                    allowBlank: false
                                }], [{
                                    columnWidth: .5,
                                    fieldLabel: this.app.i18n._('Number'),
                                    name: 'number'
                                }, new Tine.Tinebase.widgets.keyfield.ComboBox({
                                    columnWidth: .5,
                                    app: 'Projects',
                                    keyFieldName: 'projectStatus',
                                    fieldLabel: this.app.i18n._('Status'),
                                    name: 'status'
                                })]
                            ]
                        }]
                    }, {
                        xtype: 'tabpanel',
                        deferredRender: false,
                        activeTab: 0,
                        border: false,
                        height: 250,
                        form: true,
                        items: [
                            this.contactLinkPanel
                        ]
                    }]
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
                            app: 'Projects',
                            showAddNoteForm: false,
                            border: false,
                            bodyStyle: 'border:1px solid #B5B8C8;'
                        }),
                        new Tine.widgets.tags.TagPanel({
                            app: 'Projects',
                            border: false,
                            bodyStyle: 'border:1px solid #B5B8C8;'
                        })
                    ]
                }]
            }, new Tine.widgets.activities.ActivitiesTabPanel({
                app: this.appName,
                record_id: this.record.id,
                record_model: this.appName + '_Model_' + this.recordClass.getMeta('modelName')
            })]
        };
    }
});

/**
 * Projects Edit Popup
 * 
 * @param   {Object} config
 * @return  {Ext.ux.Window}
 */
Tine.Projects.ProjectEditDialog.openWindow = function (config) {
    var id = (config.record && config.record.id) ? config.record.id : 0;
    var window = Tine.WindowFactory.getWindow({
        width: 800,
        height: 470,
        name: Tine.Projects.ProjectEditDialog.prototype.windowNamePrefix + id,
        contentPanelConstructor: 'Tine.Projects.ProjectEditDialog',
        contentPanelConstructorConfig: config
    });
    return window;
};
/*
 * Tine 2.0
 * Projects combo box and store
 * 
 * @package     Projects
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */

Ext.ns('Tine.Projects');

/**
 * Project selection combo box
 * 
 * @namespace   Tine.Projects
 * @class       Tine.Projects.SearchCombo
 * @extends     Ext.form.ComboBox
 * 
 * <p>Project Search Combobox</p>
 * <p><pre>
 * TODO         make this a twin trigger field with 'clear' button?
 * TODO         add switch to filter for expired/enabled/disabled user accounts
 * </pre></p>
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schuele <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Projects.SearchCombo
 * 
 * TODO         add     forceSelection: true ?
 */
Tine.Projects.SearchCombo = Ext.extend(Tine.Tinebase.widgets.form.RecordPickerComboBox, {
    
    allowBlank: false,
    itemSelector: 'div.search-item',
    minListWidth: 200,
    
    //private
    initComponent: function(){
        this.recordClass = Tine.Projects.Model.Project;
        this.recordProxy = Tine.Projects.recordBackend;
        
        this.initTemplate();
        
        Tine.Projects.SearchCombo.superclass.initComponent.call(this);
    },
    
    /**
     * use beforequery to set query filter
     * 
     * @param {Event} qevent
     */
    onBeforeQuery: function(qevent){
        Tine.Projects.SearchCombo.superclass.onBeforeQuery.apply(this, arguments);
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
                            '<td style="height:16px">{[this.encode(values)]}</td>',
                        '</tr>',
                    '</table>',
                '</div></tpl>',
                {
                    encode: function(values) {
                        var ret = '<b>' + Ext.util.Format.htmlEncode(values.title) + '</b>';
                        if(values.number) ret += ' (' + values.number + ')';
                        return ret;
                        
                    }
                }
            );
        }
    },
    
    getValue: function() {
            return Tine.Projects.SearchCombo.superclass.getValue.call(this);
    },

    setValue: function (value) {
        return Tine.Projects.SearchCombo.superclass.setValue.call(this, value);
    }

});

Tine.widgets.form.RecordPickerManager.register('Projects', 'Project', Tine.Projects.SearchCombo);
/*
 * Tine 2.0
 * 
 * @package     Projects
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Alexander Stintzing <alex@stintzing.net>
 * @copyright   Copyright (c) 2009-2012 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.ns('Tine.Projects');

/**
 * @namespace   Tine.Projects
 * @class       Tine.Projects.AddToProjectPanel
 * @extends     Tine.widgets.dialog.AddToRecordPanel
 * @author      Alexander Stintzing <alex@stintzing.net>
 */

Tine.Projects.AddToProjectPanel = Ext.extend(Tine.widgets.dialog.AddToRecordPanel, {
    // private
    appName : 'Projects',
    recordClass: Tine.Projects.Model.Project,
    callingApp: 'Addressbook',
    callingModel: 'Contact',
    
    /**
     * @see Tine.widgets.dialog.AddToRecordPanel::isValid()
     */
    isValid: function() {
        
        var valid = true;
        if(this.searchBox.getValue() == '') {
            this.searchBox.markInvalid(this.app.i18n._('Please choose the Project to add the contacts to'));
            valid = false;
        }
        if(this.chooseRoleBox.getValue() == '') {
            this.chooseRoleBox.markInvalid(this.app.i18n._('Please select the attenders\' role'));
            valid = false;
        }
        
        return valid;
    },
    
    
    /**
     * @see Tine.widgets.dialog.AddToRecordPanel::getRelationConfig()
     */
    getRelationConfig: function() {
        var config = {
            type: this.chooseRoleBox.getValue() ? this.chooseRoleBox.getValue() : 'COWORKER'
        };
        return config;
    },

    /**
     * @see Tine.widgets.dialog.AddToRecordPanel::getFormItems()
     */
    getFormItems: function() {
        return {
            border : false,
            frame : false,
            layout : 'border',

            items : [ {
                region : 'center',
                border: false,
                frame:  false,
                layout : {
                    align: 'stretch',
                    type: 'vbox'
                },
                items: [{
                    layout:  'form',
                    margins: '10px 10px',
                    border:  false,
                    frame:   false,
                    items: [ 
                        Tine.widgets.form.RecordPickerManager.get('Projects', 'Project', {fieldLabel: this.app.i18n._('Select Project'), anchor : '100% 100%', ref: '../../../searchBox'}),
                        {
                            fieldLabel: this.app.i18n._('Role'),
                            emptyText: this.app.i18n._('Select Role'),
                            anchor: '100% 100%',
                            xtype: 'widget-keyfieldcombo',
                            app:   'Projects',
                            value: 'COWORKER',
                            keyFieldName: 'projectAttendeeRole',
                            ref: '../../../chooseRoleBox'
                        }
                        
                        ] 
                    }]

            }]
        };
    }
});

Tine.Projects.AddToProjectPanel.openWindow = function(config) {
    var window = Tine.WindowFactory.getWindow({
        modal: true,
        title : String.format(Tine.Tinebase.appMgr.get('Projects').i18n._('Adding {0} Participants to project'), config.count),
        width : 250,
        height : 150,
        contentPanelConstructor : 'Tine.Projects.AddToProjectPanel',
        contentPanelConstructorConfig : config
    });
    return window;
};
