/*!
 * Tine 2.0 - Tasks 
 * Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 * http://www.gnu.org/licenses/agpl.html AGPL Version 3
 */
/**
 * Tine 2.0
 *
 * @package     Tasks
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */

Ext.ns('Tine.Tasks.Model');

// Task model
Tine.Tasks.Model.TaskArray = Tine.Tinebase.Model.genericFields.concat([
    { name: 'id' },
    { name: 'percent', header: 'Percent' },
    { name: 'completed', type: 'date', dateFormat: Date.patterns.ISO8601Long },
    { name: 'due', type: 'date', dateFormat: Date.patterns.ISO8601Long },
    // ical common fields
    { name: 'class' },
    { name: 'description' },
    { name: 'geo' },
    { name: 'location' },
    { name: 'organizer' },
    { name: 'originator_tz' },
    { name: 'priority' },
    { name: 'status' },
    { name: 'summary' },
    { name: 'url' },
    // ical common fields with multiple appearance
    { name: 'attach' },
    { name: 'attendee' },
    { name: 'tags' },
    { name: 'comment' },
    { name: 'contact' },
    { name: 'related' },
    { name: 'resources' },
    { name: 'rstatus' },
    // scheduleable interface fields
    { name: 'dtstart', type: 'date', dateFormat: Date.patterns.ISO8601Long },
    { name: 'duration', type: 'date', dateFormat: Date.patterns.ISO8601Long },
    { name: 'recurid' },
    // scheduleable interface fields with multiple appearance
    { name: 'exdate' },
    { name: 'exrule' },
    { name: 'rdate' },
    { name: 'rrule' },
    // tine 2.0 notes field
    { name: 'notes'},
    // tine 2.0 alarms field
    { name: 'alarms'},
    // relations with other objects
    { name: 'relations'}
]);

/**
 * Task record definition
 */
Tine.Tasks.Model.Task = Tine.Tinebase.data.Record.create(Tine.Tasks.Model.TaskArray, {
    appName: 'Tasks',
    modelName: 'Task',
    idProperty: 'id',
    titleProperty: 'summary',
    // ngettext('Task', 'Tasks', n); gettext('Tasks');
    recordName: 'Task',
    recordsName: 'Tasks',
    containerProperty: 'container_id',
    // ngettext('to do list', 'to do lists', n); gettext('to do lists');
    containerName: 'to do list',
    containersName: 'to do lists'
});

/**
 * returns default account data
 *
 * @namespace Tine.Tasks.Model.Task
 * @static
 * @return {Object} default data
 */
Tine.Tasks.Model.Task.getDefaultData = function() {
    var app = Tine.Tinebase.appMgr.get('Tasks');

    return {
        'class': 'PUBLIC',
        percent: 0,
        organizer: Tine.Tinebase.registry.get('currentAccount'),
        container_id: app.getMainScreen().getWestPanel().getContainerTreePanel().getDefaultContainer()
    };
};

/**
 * @namespace Tine.Tasks.Model.Task
 *
 * get task filter
 *
 * @return {Array} filter objects
 * @static
 */
Tine.Tasks.Model.Task.getFilterModel = function() {
    var app = Tine.Tinebase.appMgr.get('Tasks');

    return [
        {label: _('Quick search'),                  field: 'query',    operators: ['contains']},
        {filtertype: 'tine.widget.container.filtermodel', app: app, recordClass: Tine.Tasks.Model.Task},
        {label: app.i18n._('Summary'),         field: 'summary' },
        {label: app.i18n._('Due Date'),        field: 'due', valueType: 'date', operators: ['within', 'before', 'after']},
        {
            label: app.i18n._('Status'),
            field: 'status',
            filtertype: 'tine.widget.keyfield.filter',
            app: app,
            defaultValue: Tine.Tasks.Model.Task.getClosedStatus(),
            keyfieldName: 'taskStatus',
            defaultOperator: 'notin'
        },
        {label: app.i18n._('Responsible'),     field: 'organizer', valueType: 'user'},
        {filtertype: 'tinebase.tag', app: app},
        {label: _('Last Modified Time'),                                                field: 'last_modified_time', valueType: 'date'},
        {label: _('Last Modified By'),                                                  field: 'last_modified_by',   valueType: 'user'},
        {label: _('Creation Time'),                                                     field: 'creation_time',      valueType: 'date'},
        {label: _('Created By'),                                                        field: 'created_by',         valueType: 'user'}
    ];
};

/**
 * @namespace Tine.Tasks.Model.Task
 *
 * get closed status ids
 *
 * @return {Array} status ids objects
 * @static
 */
Tine.Tasks.Model.Task.getClosedStatus = function() {
    var reqStatus = [];

    Tine.Tinebase.widgets.keyfield.StoreMgr.get('Tasks', 'taskStatus').each(function(status) {
        if (! status.get('is_open')) {
            reqStatus.push(status.get('id'));
        }
    }, this);

    return reqStatus;
};

/**
 * default tasks backend
 */
Tine.Tasks.JsonBackend = new Tine.Tinebase.data.RecordProxy({
    appName: 'Tasks',
    modelName: 'Task',
    recordClass: Tine.Tasks.Model.Task
});

Tine.Tasks.Model.Status = Tine.Tinebase.data.Record.create([
    { name: 'id' },
    { name: 'value' },
    { name: 'icon' },
    { name: 'system' },
    { name: 'is_open' },
    { name: 'i18nValue' }
], {
    appName: 'Tasks',
    modelName: 'Status',
    idProperty: 'id',
    titleProperty: 'i18nValue',
    // ngettext('Status', 'Status', n); gettext('Status');
    recordName: 'Status',
    recordsName: 'Status'
});/**
 * Tine 2.0
 * 
 * @package     Tasks
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */

Ext.ns('Tine', 'Tine.Tasks');

/**
 * @namespace   Tine.Tasks
 * @class       Tine.Tasks.Application
 * @extends     Tine.Tinebase.Application
 * Tasks Application Object <br>
 * 
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 */
Tine.Tasks.Application = Ext.extend(Tine.Tinebase.Application, {
    
    /**
     * auto hook text _('New Task')
     */
    addButtonText: 'New Task'
});

// default mainscreen
Tine.Tasks.MainScreen = Ext.extend(Tine.widgets.MainScreen, {
    activeContentType: 'Task'
});

Tine.Tasks.TaskTreePanel = function(config) {
    Ext.apply(this, config);
    
    this.id = 'TasksTreePanel';
    this.recordClass = Tine.Tasks.Model.Task;
    
    this.filterMode = 'filterToolbar';
    Tine.Tasks.TaskTreePanel.superclass.constructor.call(this);
};
Ext.extend(Tine.Tasks.TaskTreePanel, Tine.widgets.container.TreePanel, {
    afterRender: function() {
        this.supr().afterRender.apply(this, arguments);
    }
});

Tine.Tasks.TaskFilterPanel = function(config) {
    Ext.apply(this, config);
    Tine.Tasks.TaskFilterPanel.superclass.constructor.call(this);
};
Ext.extend(Tine.Tasks.TaskFilterPanel, Tine.widgets.persistentfilter.PickerPanel, {
    filter: [{field: 'model', operator: 'equals', value: 'Tasks_Model_TaskFilter'}]
});
/*
 * Tine 2.0
 * 
 * @package     Tasks
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */

Ext.namespace('Tine.Tasks');

/**
 * Tasks grid panel
 * 
 * @namespace   Tine.Tasks
 * @class       Tine.Tasks.TaskGridPanel
 * @extends     Tine.widgets.grid.GridPanel
 * 
 * <p>Tasks Grid Panel</p>
 * <p><pre>
 * </pre></p>
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2007-2008 Metaways Infosystems GmbH (http://www.metaways.de)
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Tasks.TaskGridPanel
 */
Tine.Tasks.TaskGridPanel = Ext.extend(Tine.widgets.grid.GridPanel, {
    /**
     * record class
     * @cfg {Tine.Tasks.Model.Task} recordClass
     */
    recordClass: Tine.Tasks.Model.Task,
    
    /**
     * @private grid cfg
     */
    defaultSortInfo: {field: 'due', dir: 'ASC'},
    gridConfig: {
        clicksToEdit: 'auto',
        quickaddMandatory: 'summary',
        resetAllOnNew: false,
        autoExpandColumn: 'summary',
        // drag n drop
        enableDragDrop: true,
        ddGroup: 'containerDDGroup'
    },
    
    // specialised translations
    // ngettext('Do you really want to delete the selected task?', 'Do you really want to delete the selected tasks?', n);
    i18nDeleteQuestion: ['Do you really want to delete the selected task?', 'Do you really want to delete the selected tasks?'],
    
    /**
     * @private
     */
    initComponent: function() {
        this.recordProxy = Tine.Tasks.JsonBackend;
        
        //this.actionToolbarItems = this.getToolbarItems();
        this.gridConfig.cm = this.getColumnModel();
        this.initFilterToolbar();
        
        this.plugins = this.plugins || [];
        this.plugins.push(/*this.action_showClosedToggle,*/ this.filterToolbar);
        
        Tine.Tasks.TaskGridPanel.superclass.initComponent.call(this);
        
        // the editGrids onEditComplete calls the focusCell after a edit operation
        // this leads to a 'flicker' effect we dont want!
        // mhh! but disabling this, breaks keynav 
        //this.grid.view.focusCell = Ext.emptyFn;
    },
    
    /**
     * initialises filter toolbar
     * @private
     */
    initFilterToolbar: function() {
        this.filterToolbar = new Tine.widgets.grid.FilterPanel({
            recordClass: this.recordClass,
            app: this.app,
            filterModels: Tine.Tasks.Model.Task.getFilterModel(),
            defaultFilter: 'query',
            filters: [
                {field: 'container_id', operator: 'equals', value: {path: Tine.Tinebase.container.getMyNodePath()}}
            ],
            plugins: [
                new Tine.widgets.grid.FilterToolbarQuickFilterPlugin()
            ]
        });
    },

    
    /**
     * returns cm
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
        {   id: 'lead_name', header: this.app.i18n._('Lead'), dataIndex: 'relations', width: 175, sortable: false, hidden: true, renderer: this.leadRenderer },
        {
            id: 'summary',
            header: this.app.i18n._("Summary"),
            width: 400,
            dataIndex: 'summary',
            quickaddField: new Ext.form.TextField({
                emptyText: this.app.i18n._('Add a task...')
            })
        }, {
            id: 'due',
            header: this.app.i18n._("Due Date"),
            width: 145,
            dataIndex: 'due',
            renderer: Tine.Tinebase.common.dateTimeRenderer,
            editor: new Ext.ux.form.DateTimeField({
                defaultTime: '12:00',
                allowBlank: true
            }),
            quickaddField: new Ext.ux.form.DateTimeField({
                defaultTime: '12:00',
                allowBlank: true
            })
        }, {
            id: 'priority',
            header: this.app.i18n._("Priority"),
            width: 65,
            dataIndex: 'priority',
            renderer: Tine.Tinebase.widgets.keyfield.Renderer.get('Tasks', 'taskPriority'),
            editor: {
                xtype: 'widget-keyfieldcombo',
                app: 'Tasks',
                keyFieldName: 'taskPriority'
            },
            quickaddField: new Tine.Tinebase.widgets.keyfield.ComboBox({
                app: 'Tasks',
                keyFieldName: 'taskPriority',
                value: 'NORMAL'
            })
        }, {
            id: 'percent',
            header: this.app.i18n._("Percent"),
            width: 50,
            dataIndex: 'percent',
            renderer: Ext.ux.PercentRenderer,
            editor: new Ext.ux.PercentCombo({
                autoExpand: true,
                blurOnSelect: true
            }),
            quickaddField: new Ext.ux.PercentCombo({
                autoExpand: true
            })
        }, {
            id: 'status',
            header: this.app.i18n._("Status"),
            width: 85,
            dataIndex: 'status',
            renderer: Tine.Tinebase.widgets.keyfield.Renderer.get('Tasks', 'taskStatus'),
            editor: {
                xtype: 'widget-keyfieldcombo',
                app: 'Tasks',
                keyFieldName: 'taskStatus'
            },
            quickaddField: new Tine.Tinebase.widgets.keyfield.ComboBox({
                app: 'Tasks',
                keyFieldName: 'taskStatus',
                value: 'NEEDS-ACTION'
            })
        }, {
            id: 'creation_time',
            header: this.app.i18n._("Creation Time"),
            hidden: true,
            width: 90,
            dataIndex: 'creation_time',
            renderer: Tine.Tinebase.common.dateTimeRenderer
        }, {
            id: 'completed',
            header: this.app.i18n._("Completed"),
            hidden: true,
            width: 90,
            dataIndex: 'completed',
            renderer: Tine.Tinebase.common.dateTimeRenderer
        }, {
            id: 'organizer',
            header: this.app.i18n._('Responsible'),
            width: 200,
            dataIndex: 'organizer',
            renderer: Tine.Tinebase.common.accountRenderer,
            quickaddField: Tine.widgets.form.RecordPickerManager.get('Addressbook', 'Contact', {
                userOnly: true,
                useAccountRecord: true,
                blurOnSelect: true,
                selectOnFocus: true,
                allowEmpty: true,
                value: Tine.Tinebase.registry.get('currentAccount')
            })
        }]
        // TODO add customfields and modlog columns, atm they break the layout :(
        //.concat(this.getModlogColumns().concat(this.getCustomfieldColumns()))
        });
    },
    
    /**
     * return lead name for first linked Crm_Model_Lead
     * 
     * @param {Object} data
     * @return {String} lead name
     */
    leadRenderer: function(data) {
    
        if( Ext.isArray(data) && data.length > 0) {
            var index = 0;
            // get correct relation type from data (contact) array and show first matching record (org_name + n_fileas)
            while (index < data.length && data[index].related_model != 'Crm_Model_Lead') {
                index++;
            }
            if (data[index]) {
                var name = (data[index].related_record.lead_name !== null ) ? data[index].related_record.lead_name : '';
                return Ext.util.Format.htmlEncode(name);
            }
        }
    },    

    /**
     * Return CSS class to apply to rows depending upon due status
     * 
     * @param {Tine.Tasks.Model.Task} record
     * @param {Integer} index
     * @return {String}
     */
    getViewRowClass: function(record, index) {
        var due = record.get('due');
         
        var className = '';
        
        if(record.get('status') == 'COMPLETED') {
            className += 'tasks-grid-completed';
        } else  if (due) {
            var dueDay = due.format('Y-m-d');
            var today = new Date().format('Y-m-d');

            if (dueDay == today) {
                className += 'tasks-grid-duetoday';
            } else if (dueDay < today) {
                className += 'tasks-grid-overdue';
            }
            
        }
        return className;
    }
});
/*
 * Tine 2.0
 * 
 * @package     Tasks
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2007-2009 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.namespace('Tine.Tasks');

/**
 * @namespace   Tine.Tasks
 * @class       Tine.Tasks.TaskEditDialog
 * @extends     Tine.widgets.dialog.EditDialog
 * 
 * <p>Tasks Edit Dialog</p>
 * <p>
 * TODO         refactor this: remove initRecord/containerId/relatedApp, 
 *              adopt to normal edit dialog flow and add getDefaultData to task model
 * </p>
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2007-2009 Metaways Infosystems GmbH (http://www.metaways.de)
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Tasks.TaskEditDialog
 */
 Tine.Tasks.TaskEditDialog = Ext.extend(Tine.widgets.dialog.EditDialog, {
    /**
     * @cfg {Number} containerId
     */
    containerId: -1,
    
    /**
     * @cfg {String} relatedApp
     */
    relatedApp: '',
    
    /**
     * @private
     */
    labelAlign: 'side',
    
    /**
     * @private
     */
    windowNamePrefix: 'TasksEditWindow_',
    appName: 'Tasks',
    recordClass: Tine.Tasks.Model.Task,
    recordProxy: Tine.Tasks.JsonBackend,
    showContainerSelector: true,
    tbarItems: [{xtype: 'widget-activitiesaddbutton'}],
    
    /**
     * @private
     */
    initComponent: function() {
        
        if(!this.record) {
            this.record = new this.recordClass(this.recordClass.getDefaultData(), 0);
        }
        
        this.alarmPanel = new Tine.widgets.dialog.AlarmPanel({});
        Tine.Tasks.TaskEditDialog.superclass.initComponent.call(this);
    },
    
    /**
     * executed when record is loaded
     * @private
     */
    onRecordLoad: function() {
        // interrupt process flow until dialog is rendered
        if (! this.rendered) {
            this.onRecordLoad.defer(250, this);
            return;
        }
        
        Tine.Tasks.TaskEditDialog.superclass.onRecordLoad.apply(this, arguments);
        this.handleCompletedDate();
        
        // update tabpanels
        this.alarmPanel.onRecordLoad(this.record);
    },
    
    /**
     * executed when record is updated
     * @private
     */
    onRecordUpdate: function() {
        Tine.Tasks.TaskEditDialog.superclass.onRecordUpdate.apply(this, arguments);
        this.alarmPanel.onRecordUpdate(this.record);
    },
    
    /**
     * handling for the completed field
     * @private
     */
    handleCompletedDate: function() {
        
        var statusStore = Tine.Tinebase.widgets.keyfield.StoreMgr.get('Tasks', 'taskStatus'),
            status = this.getForm().findField('status').getValue(),
            statusRecord = statusStore.getById(status),
            completedField = this.getForm().findField('completed');
        
        if (statusRecord) {
            if (statusRecord.get('is_open') !== 0) {
                completedField.setValue(null);
                completedField.setDisabled(true);
            } else {
                if (! Ext.isDate(completedField.getValue())){
                    completedField.setValue(new Date());
                }
                completedField.setDisabled(false);
            }
        }
        
    },
    
    /**
     * checks if form data is valid
     * 
     * @return {Boolean}
     */
    isValid: function() {
        var isValid = true;
        
        var dueField = this.getForm().findField('due'),
            dueDate = dueField.getValue(),
            alarms = this.alarmPanel.alarmGrid.getFromStoreAsArray();
            
        if (! Ext.isEmpty(alarms) && ! Ext.isDate(dueDate)) {
            dueField.markInvalid(this.app.i18n._('You have to supply a due date, because an alarm ist set!'));
            
            isValid = false;
        }
        
        return isValid && Tine.Tasks.TaskEditDialog.superclass.isValid.apply(this, arguments);
    },
    
    /**
     * returns dialog
     * 
     * NOTE: when this method gets called, all initalisation is done.
     * @private
     */
    getFormItems: function() {
        return {
            xtype: 'tabpanel',
            border: false,
            plain:true,
            activeTab: 0,
            border: false,
            plugins: [{
                ptype : 'ux.tabpanelkeyplugin'
            }],
            items:[{
                title: this.app.i18n.n_('Task', 'Tasks', 1),
                autoScroll: true,
                border: false,
                frame: true,
                layout: 'border',
                items: [{
                    region: 'center',
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
                        fieldLabel: this.app.i18n._('Summary'),
                        name: 'summary',
                        listeners: {render: function(field){field.focus(false, 250);}},
                        allowBlank: false
                    }], [new Ext.ux.form.DateTimeField({
                            allowBlank: true,
                            defaultTime: '12:00',
                            fieldLabel: this.app.i18n._('Due date'),
                            name: 'due'
                        }), 
                        new Tine.Tinebase.widgets.keyfield.ComboBox({
                            fieldLabel: this.app.i18n._('Priority'),
                            name: 'priority',
                            app: 'Tasks',
                            keyFieldName: 'taskPriority',
                            value: 'NORMAL'
                        }),
                        Tine.widgets.form.RecordPickerManager.get('Addressbook', 'Contact', {
                            userOnly: true,
                            fieldLabel: this.app.i18n._('Organizer'),
                            emptyText: _('Add Responsible ...'),
                            useAccountRecord: true,
                            name: 'organizer',
                            allowEmpty: true
                        })
                    ], [{
                        columnWidth: 1,
                        fieldLabel: this.app.i18n._('Notes'),
                        emptyText: this.app.i18n._('Enter description...'),
                        name: 'description',
                        xtype: 'textarea',
                        height: 200
                    }], [
                        new Ext.ux.PercentCombo({
                            fieldLabel: this.app.i18n._('Percentage'),
                            editable: false,
                            name: 'percent'
                        }), 
                        new Tine.Tinebase.widgets.keyfield.ComboBox({
                            app: 'Tasks',
                            keyFieldName: 'taskStatus',
                            fieldLabel: this.app.i18n._('Status'),
                            name: 'status',
                            listeners: {scope: this, 'change': this.handleCompletedDate}
                        }), 
                        new Ext.ux.form.DateTimeField({
                            allowBlank: true,
                            defaultTime: '12:00',
                            fieldLabel: this.app.i18n._('Completed'),
                            name: 'completed'
                        })
                    ]]
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
                        new Tine.widgets.activities.ActivitiesPanel({
                            app: 'Tasks',
                            showAddNoteForm: false,
                            border: false,
                            bodyStyle: 'border:1px solid #B5B8C8;'
                        }),
                        new Tine.widgets.tags.TagPanel({
                            app: 'Tasks',
                            border: false,
                            bodyStyle: 'border:1px solid #B5B8C8;'
                        })
                    ]
                }]
            }, new Tine.widgets.activities.ActivitiesTabPanel({
                app: this.appName,
                record_id: (this.record) ? this.record.id : '',
                record_model: this.appName + '_Model_' + this.recordClass.getMeta('modelName')
            }), this.alarmPanel
            ]
        };
    }
});

/**
 * Tasks Edit Popup
 * 
 * @param   {Object} config
 * @return  {Ext.ux.Window}
 */
Tine.Tasks.TaskEditDialog.openWindow = function (config) {
    var id = (config.record && config.record.id) ? config.record.id : 0;
    var window = Tine.WindowFactory.getWindow({
        width: 900,
        height: 490,
        name: Tine.Tasks.TaskEditDialog.prototype.windowNamePrefix + id,
        contentPanelConstructor: 'Tine.Tasks.TaskEditDialog',
        contentPanelConstructorConfig: config
    });
    return window;
};
