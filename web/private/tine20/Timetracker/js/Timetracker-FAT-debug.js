/*!
 * Tine 2.0 - Timetracker 
 * Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 * http://www.gnu.org/licenses/agpl.html AGPL Version 3
 */
/**
 * Tine 2.0
 * 
 * @package     Timetracker
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2007-2008 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.ns('Tine.Timetracker');

/**
 * handles minutes to time conversions
 * @class Tine.Timetracker.DurationSpinner
 * @extends Ext.ux.form.Spinner
 */
Tine.Timetracker.DurationSpinner = Ext.extend(Ext.ux.form.Spinner,  {
    
    /**
     * Set to empty value if value equals 0
     * @cfg emptyOnZero
     */
    emptyOnZero: null,
    
    initComponent: function() {
        this.preventMark = false;
        this.strategy = new Ext.ux.form.Spinner.TimeStrategy({
            incrementValue : 15
        });
        this.format = this.strategy.format;
    },
    
    setValue: function(value) {
        if(! value || value == '00:00') {
            value = this.emptyOnZero ? '' : '00:00';
        } else if(! value.toString().match(/:/)) {
            var time = new Date(0);
            var hours = Math.floor(value / 60);
            var minutes = value - hours * 60;
            
            time.setHours(hours);
            time.setMinutes(minutes);
            
            value = Ext.util.Format.date(time, this.format);
        }
        
        Tine.Timetracker.DurationSpinner.superclass.setValue.call(this, value);
    },
    
    validateValue: function(value) {
        var time = Date.parseDate(value, this.format);
        return Ext.isDate(time);
    },
    
    getValue: function() {
        var value = Tine.Timetracker.DurationSpinner.superclass.getValue.call(this);
        value = value.replace(',', '.');
        
        if(value && typeof value == 'string') {
            if (value.search(/:/) != -1) {
                var parts = value.split(':');
                parts[0] = parts[0].length == 1 ? '0' + parts[0] : parts[0];
                parts[1] = parts[1].length == 1 ? '0' + parts[1] : parts[1];
                value = parts.join(':');
                
                var time = Date.parseDate(value, this.format);
                if (! time) {
                    this.markInvalid(_('Not a valid time'));
                    return;
                } else {
                    value = time.getHours() * 60 + time.getMinutes();
                }
            } else if (value > 0) {
                if (value < 24) {
                    value = value * 60;
                }
            } else {
                this.markInvalid(_('Not a valid time'));
                return;
            }
        }
        this.setValue(value);
        return value;
    }
});

Ext.reg('tinedurationspinner', Tine.Timetracker.DurationSpinner);
/**
 * Tine 2.0
 * 
 * @package     Timetracker
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2007-2012 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.ns('Tine.Timetracker', 'Tine.Timetracker.Model');

/**
 * @type {Array}
 * Timesheet model fields
 */
Tine.Timetracker.Model.TimesheetArray = Tine.Tinebase.Model.genericFields.concat([
    { name: 'id' },
    { name: 'account_id' },
    { name: 'timeaccount_id' },
    { name: 'start_date', type: 'date', dateFormat: Date.patterns.ISO8601Short},
    { name: 'start_time', type: 'date', dateFormat: Date.patterns.ISO8601Time },
    { name: 'duration' },
    { name: 'description' },
    { name: 'is_billable', type: 'bool' },
    { name: 'is_billable_combined', type: 'bool' }, // ts & ta is_billable
    { name: 'is_cleared', type: 'bool' },
    { name: 'is_cleared_combined', type: 'bool' }, // ts is_cleared & ta status == 'billed'
    { name: 'billed_in' },
    // tine 2.0 notes + tags
    { name: 'notes'},
    { name: 'tags' },
    { name: 'customfields'}
    // relations
    // TODO fix this, relations do not work yet in TS edit dialog (without admin/manage privileges)
    //{ name: 'relations'}
]);

/**
 * @type {Tine.Tinebase.data.Record}
 * Timesheet record definition
 */
Tine.Timetracker.Model.Timesheet = Tine.Tinebase.data.Record.create(Tine.Timetracker.Model.TimesheetArray, {
    appName: 'Timetracker',
    modelName: 'Timesheet',
    idProperty: 'id',
    titleProperty: null,
    // ngettext('Timesheet', 'Timesheets', n);
    recordName: 'Timesheet',
    recordsName: 'Timesheets',
    containerProperty: 'timeaccount_id',
    // ngettext('timesheets list', 'timesheets lists', n);
    containerName: 'All Timesheets',
    containersName: 'timesheets lists',
    getTitle: function() {
        var timeaccount = this.get('timeaccount_id'),
            description = Ext.util.Format.ellipsis(this.get('description'), 30, true),
            timeaccountTitle = '';
        
        if (timeaccount) {
            if (typeof(timeaccount.get) !== 'function') {
                timeaccount = new Tine.Timetracker.Model.Timeaccount(timeaccount);
            }
            timeaccountTitle = timeaccount.getTitle();
        }
        
        timeaccountTitle = timeaccountTitle ? '[' + timeaccountTitle + '] ' : '';
        return timeaccountTitle + description;
    },
    copyOmitFields: ['billed_in', 'is_cleared']
});

Tine.Timetracker.Model.Timesheet.getDefaultData = function() {
    return {
        account_id: Tine.Tinebase.registry.get('currentAccount'),
        duration:   '00:30',
        start_date: new Date(),
        is_billable: true,
        timeaccount_id: {account_grants: {bookOwnGrant: true}}
    };
};

Tine.Timetracker.Model.Timesheet.getFilterModel = function() {
    var app = Tine.Tinebase.appMgr.get('Timetracker');
    
    return [
        {label: app.i18n._('Quick search'), field: 'query',    operators: ['contains']}, // query only searches description
        {label: app.i18n._('Account'),      field: 'account_id', valueType: 'user'},
        {label: app.i18n._('Date'),         field: 'start_date', valueType: 'date', pastOnly: true},
        {label: app.i18n._('Description'),  field: 'description', defaultOperator: 'contains'},
        {label: app.i18n._('Billable'),     field: 'is_billable_combined', valueType: 'bool', defaultValue: true },
        {label: app.i18n._('Cleared'),      field: 'is_cleared_combined',  valueType: 'bool', defaultValue: false },
        {label: _('Last Modified Time'),                                                field: 'last_modified_time', valueType: 'date'},
        {label: _('Last Modified By'),                                                  field: 'last_modified_by',   valueType: 'user'},
        {label: _('Creation Time'),                                                     field: 'creation_time',      valueType: 'date'},
        {label: _('Created By'),                                                        field: 'created_by',         valueType: 'user'},
        {filtertype: 'tinebase.tag', app: app},
        {filtertype: 'timetracker.timeaccount'}
    ];
};


/**
 * @type {Array}
 * Timeaccount model fields
 */
Tine.Timetracker.Model.TimeaccountArray = Tine.Tinebase.Model.genericFields.concat([
    { name: 'id' },
    { name: 'container_id' },
    { name: 'title' },
    { name: 'number' },
    { name: 'description' },
    { name: 'budget' },
    { name: 'budget_unit' },
    { name: 'price' },
    { name: 'price_unit' },
    { name: 'is_open', type: 'bool'},
    { name: 'is_billable', type: 'bool' },
    { name: 'billed_in' },
    { name: 'status' },
    { name: 'deadline' },
    { name: 'account_grants'},
    { name: 'grants'},
    // tine 2.0 notes + tags
    { name: 'notes'},
    { name: 'tags' },
    { name: 'customfields'},
    // relations
    { name: 'relations'}
]);

/**
 * @type {Tine.Tinebase.data.Record}
 * Timesheet record definition
 */
Tine.Timetracker.Model.Timeaccount = Tine.Tinebase.data.Record.create(Tine.Timetracker.Model.TimeaccountArray, {
    appName: 'Timetracker',
    modelName: 'Timeaccount',
    idProperty: 'id',
    titleProperty: 'title',
    // ngettext('Time Account', 'Time Accounts', n);
    recordName: 'Time Account',
    recordsName: 'Time Accounts',
    containerProperty: 'container_id',
    // ngettext('timeaccount list', 'timeaccount lists', n);
    containerName: 'All Timeaccounts',
    containersName: 'timeaccount lists',
    getTitle: function() {
        var closedText = this.get('is_open') ? '' : (' (' + Tine.Tinebase.appMgr.get('Timetracker').i18n._('closed') + ')');
        
        return this.get('number') ? (this.get('number') + ' - ' + this.get('title') + closedText) : '';
    }
});

Tine.Timetracker.Model.Timeaccount.getDefaultData = function() {
    return {
        is_open: 1,
        is_billable: true
    };
};

Tine.Timetracker.Model.Timeaccount.getFilterModel = function() {
    var app = Tine.Tinebase.appMgr.get('Timetracker');

    var filters = [
        {label: _('Quick search'),              field: 'query',       operators: ['contains']},
        {label: app.i18n._('Number'),           field: 'number'       },
        {label: app.i18n._('Title'),            field: 'title'        },
        {label: app.i18n._('Description'),      field: 'description', operators: ['contains']},
        {label: app.i18n._('Billed'),           field: 'status',      filtertype: 'timetracker.timeaccountbilled'},
        {label: app.i18n._('Status'),           field: 'is_open',     filtertype: 'timetracker.timeaccountstatus'},
        {label: _('Last Modified Time'),        field: 'last_modified_time', valueType: 'date'},
        {label: _('Last Modified By'),          field: 'last_modified_by',   valueType: 'user'},
        {label: _('Creation Time'),             field: 'creation_time',      valueType: 'date'},
        {label: _('Created By'),                field: 'created_by',         valueType: 'user'},
        {label: app.i18n._('Booking deadline'), field: 'deadline'},
        {filtertype: 'tinebase.tag', app: app}
    ];
    
    if(Tine.Tinebase.appMgr.get('Sales')) {
        filters.push({filtertype: 'timetracker.timeaccountcontract'});
    }
    
    return filters;
};

/**
 * Model of a grant
 */
Tine.Timetracker.Model.TimeaccountGrant = Ext.data.Record.create([
    {name: 'id'},
    {name: 'account_id'},
    {name: 'account_type'},
    {name: 'account_name'},
    {name: 'bookOwnGrant',        type: 'boolean'},
    {name: 'viewAllGrant',        type: 'boolean'},
    {name: 'bookAllGrant',        type: 'boolean'},
    {name: 'manageBillableGrant', type: 'boolean'},
    {name: 'exportGrant',         type: 'boolean'},
    {name: 'adminGrant',          type: 'boolean'}
]);
/*
 * Tine 2.0
 * 
 * @package     Timetracker
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 */
 
Ext.ns('Tine.Timetracker');

/**
 * @namespace   Tine.Timetracker
 * @class       Tine.Timetracker.Application
 * @extends     Tine.Tinebase.Application
 * Timetracker Application Object <br>
 * 
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 */
Tine.Timetracker.Application = Ext.extend(Tine.Tinebase.Application, {
    init: function() {
        Tine.Timetracker.Application.superclass.init.apply(this, arguments);
        
        Ext.ux.ItemRegistry.registerItem('Tine.widgets.grid.GridPanel.addButton', {
            text: this.i18n._('New Timesheet'), 
            iconCls: 'TimetrackerTimesheet',
            scope: this,
            handler: function() {
                var ms = this.getMainScreen(),
                    cp = ms.getCenterPanel('Timesheet');
                    
                cp.onEditInNewWindow.call(cp, {});
            }
        });
    }
});

/**
 * @namespace   Tine.Timetracker
 * @class       Tine.Timetracker.MainScreen
 * @extends     Tine.widgets.MainScreen
 * MainScreen of the Timetracker Application <br>
 * 
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * 
 * @constructor
 */
Tine.Timetracker.MainScreen = Ext.extend(Tine.widgets.MainScreen, {
    activeContentType: 'Timesheet',
    contentTypes: [
        {model: 'Timesheet', requiredRight: null, singularContainerMode: true},
        {model: 'Timeaccount', requiredRight: 'manage', singularContainerMode: true}]
});

/**
 * default filter panels
 */
Tine.Timetracker.TimesheetFilterPanel = function(config) {
    Ext.apply(this, config);
    Tine.Timetracker.TimesheetFilterPanel.superclass.constructor.call(this);
};

Ext.extend(Tine.Timetracker.TimesheetFilterPanel, Tine.widgets.persistentfilter.PickerPanel, {
    filter: [{field: 'model', operator: 'equals', value: 'Timetracker_Model_TimesheetFilter'}]
});

Tine.Timetracker.TimeaccountFilterPanel = function(config) {
    Ext.apply(this, config);
    Tine.Timetracker.TimeaccountFilterPanel.superclass.constructor.call(this);
};

Ext.extend(Tine.Timetracker.TimeaccountFilterPanel, Tine.widgets.persistentfilter.PickerPanel, {
    filter: [{field: 'model', operator: 'equals', value: 'Timetracker_Model_TimeaccountFilter'}]
});



/**
 * default timesheets backend
 */
Tine.Timetracker.timesheetBackend = new Tine.Tinebase.data.RecordProxy({
    appName: 'Timetracker',
    modelName: 'Timesheet',
    recordClass: Tine.Timetracker.Model.Timesheet
});

/**
 * default timeaccounts backend
 */
Tine.Timetracker.timeaccountBackend = new Tine.Tinebase.data.RecordProxy({
    appName: 'Timetracker',
    modelName: 'Timeaccount',
    recordClass: Tine.Timetracker.Model.Timeaccount
});
/* 
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2012 Metaways Infosystems GmbH (http://www.metaways.de)
 */

Ext.ns('Tine.Timetracker');

/**
 * @namespace   Tine.Timetracker
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @class       Tine.Timetracker.TimeaccountPickerCombo
 * @extends     Tine.Tinebase.widgets.form.RecordPickerComboBox
 * 
 * adds show closed handling
 */

Tine.Timetracker.TimeaccountPickerCombo = Ext.extend(Tine.Tinebase.widgets.form.RecordPickerComboBox, {
    /**
     * @cfg {Bool} showClosed
     */
    showClosed: false,
    
    /**
     * @property showClosedBtn
     * @type Ext.Button
     */
    showClosedBtn: null,
    
    sortBy: 'number',
    
    initComponent: function() {
        this.recordProxy = Tine.Timetracker.timeaccountBackend;
        this.recordClass = Tine.Timetracker.Model.Timeaccount;
        
        Tine.Timetracker.TimeaccountPickerCombo.superclass.initComponent.apply(this, arguments);
    },
    
    initList: function() {
        Tine.Timetracker.TimeaccountPickerCombo.superclass.initList.apply(this, arguments);
        
        if (this.pageTb && ! this.showClosedBtn) {
            this.showClosedBtn = new Tine.widgets.grid.FilterButton({
                text: this.app.i18n._('Show closed'),
                iconCls: 'action_showArchived',
                field: 'is_open',
                invert: true,
                pressed: this.showClosed,
                scope: this,
                handler: function() {
                    this.showClosed = this.showClosedBtn.pressed;
                    this.store.load();
                }
                
            });
            
            this.pageTb.add('-', this.showClosedBtn);
            this.pageTb.doLayout();
        }
    },
    
    /**
     * apply showClosed value
     */
    onStoreBeforeLoadRecords: function(o, options, success, store) {
        if (Tine.Timetracker.TimeaccountPickerCombo.superclass.onStoreBeforeLoadRecords.apply(this, arguments) !== false) {
            if (this.showClosedBtn) {
                this.showClosedBtn.setValue(options.params.filter);
            }
        }
    },
    
    /**
     * append showClosed value
     */
    onBeforeLoad: function (store, options) {
        Tine.Timetracker.TimeaccountPickerCombo.superclass.onBeforeLoad.apply(this, arguments);

        if (this.showClosedBtn) {
            Ext.each(store.baseParams.filter, function(filter, idx) {
                if (filter.field == 'is_open'){
                    store.baseParams.filter.remove(filter);
                }
            }, this);
            
            if (this.showClosedBtn.getValue().value === true) {
                store.baseParams.filter.push(this.showClosedBtn.getValue());
            }
        }
    }
});

Tine.widgets.form.RecordPickerManager.register('Timetracker', 'Timeaccount', Tine.Timetracker.TimeaccountPickerCombo);/**
 * Tine 2.0
 * 
 * @package     Timetracker
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 */
 
Ext.ns('Tine.Timetracker');

Tine.Timetracker.TimeAccountFilterModel = Ext.extend(Tine.widgets.grid.ForeignRecordFilter, {
    
    /**
     * @cfg {Record} foreignRecordClass needed for explicit defined filters
     */
    foreignRecordClass : Tine.Timetracker.Model.Timeaccount,
    
    /**
     * @cfg {String} linkType {relation|foreignId} needed for explicit defined filters
     */
    linkType: 'foreignId',
    
    /**
     * @cfg {String} filterName server side filterGroup Name, needed for explicit defined filters
     */
    filterName: 'TimeaccountFilter',
    
    /**
     * @cfg {String} ownField for explicit filterRow
     */
    ownField: 'timeaccount_id',
    
    /**
     * @private
     */
    initComponent: function() {
        this.app = Tine.Tinebase.appMgr.get('Timetracker');
        this.label = this.app.i18n.n_hidden(this.foreignRecordClass.getMeta('recordName'), this.foreignRecordClass.getMeta('recordsName'), 1);
        
        this.pickerConfig = this.pickerConfig || {};
        
        Tine.Timetracker.TimeAccountFilterModel.superclass.initComponent.call(this);
    }
});

Tine.widgets.grid.FilterToolbar.FILTERS['timetracker.timeaccount'] = Tine.Timetracker.TimeAccountFilterModel;/**
 * Tine 2.0
 * 
 * @package     Timetracker
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 */
 
Ext.ns('Tine.Timetracker');

Tine.Timetracker.TimeAccountStatusFilterModel = Ext.extend(Tine.widgets.grid.FilterModel, {
    field: 'is_open',
    valueType: 'bool',
    defaultValue: 1,
    
    /**
     * @private
     */
    initComponent: function() {
        Tine.Timetracker.TimeAccountStatusFilterModel.superclass.initComponent.call(this);
        
        this.app = Tine.Tinebase.appMgr.get('Timetracker');
        this.label = this.label ? this.label : this.app.i18n._("Time Account - Status");
        this.operators = ['equals'];
    },
   
    /**
     * value renderer
     * 
     * @param {Ext.data.Record} filter line
     * @param {Ext.Element} element to render to 
     */
    valueRenderer: function(filter, el) {
        // value
        var value = new Ext.form.ComboBox({
            filter: filter,
            width: 200,
            id: 'tw-ftb-frow-valuefield-' + filter.id,
            value: filter.data.value ? filter.data.value : this.defaultValue,
            renderTo: el,
            mode: 'local',
            forceSelection: true,
            blurOnSelect: true,
            triggerAction: 'all',
            store: [[0, this.app.i18n._('closed')], [1, this.app.i18n._('open')]]
        });
        value.on('specialkey', function(field, e){
             if(e.getKey() == e.ENTER){
                 this.onFiltertrigger();
             }
        }, this);
        
        return value;
    }
});
Tine.widgets.grid.FilterToolbar.FILTERS['timetracker.timeaccountstatus'] = Tine.Timetracker.TimeAccountStatusFilterModel;
/**
 * Tine 2.0
 * 
 * @package     Timetracker
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 */
 
Ext.ns('Tine.Timetracker');

Tine.Timetracker.TimeAccountBilledFilterModel = Ext.extend(Tine.widgets.grid.FilterModel, {
    field: 'timeaccount_status',
    valueType: 'string',
    defaultValue: 'to bill',
    
    /**
     * @private
     */
    initComponent: function() {
        Tine.Timetracker.TimeAccountBilledFilterModel.superclass.initComponent.call(this);
        
        this.app = Tine.Tinebase.appMgr.get('Timetracker');
        this.label = this.label ? this.label : this.app.i18n._("Time Account - Billed");
        this.operators = ['equals'];
    },
   
    /**
     * value renderer
     * 
     * @param {Ext.data.Record} filter line
     * @param {Ext.Element} element to render to 
     */
    valueRenderer: function(filter, el) {
        // value
        var value = new Ext.form.ComboBox({
            filter: filter,
            width: 200,
            id: 'tw-ftb-frow-valuefield-' + filter.id,
            value: filter.data.value ? filter.data.value : this.defaultValue,
            renderTo: el,
            mode: 'local',
            forceSelection: true,
            blurOnSelect: true,
            triggerAction: 'all',
            store: [
                ['not yet billed', this.app.i18n._('not yet billed')], 
                ['to bill', this.app.i18n._('to bill')],
                ['billed', this.app.i18n._('billed')]
            ]
        });
        value.on('specialkey', function(field, e){
             if(e.getKey() == e.ENTER){
                 this.onFiltertrigger();
             }
        }, this);
        
        return value;
    }
});
Tine.widgets.grid.FilterToolbar.FILTERS['timetracker.timeaccountbilled'] = Tine.Timetracker.TimeAccountBilledFilterModel;
/**
 * Tine 2.0
 * 
 * @package     Timetracker
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Alexander Stintzing <a.stintzing@metaways.de>
 * @copyright   Copyright (c) 2012 Metaways Infosystems GmbH (http://www.metaways.de)
 */
 
Ext.ns('Tine.Timetracker');

/**
 * @namespace   Tine.Timetracker
 * @class       Tine.Timetracker.TimeaccountContractFilterModel
 * @extends     Tine.widgets.grid.ForeignRecordFilter
 * 
 * @author      Alexander Stintzing <a.stintzing@metaways.de>
 */
Tine.Timetracker.TimeaccountContractFilterModel = Ext.extend(Tine.widgets.grid.ForeignRecordFilter, {
    
    // private
    field: 'contract',
    valueType: 'relation',
    
    /**
     * @private
     */
    initComponent: function() {
        this.app = Tine.Tinebase.appMgr.get('Timetracker');
        this.label = this.app.i18n._('Contract');
        this.foreignRecordClass = 'Sales.Contract',
        this.pickerConfig = {emptyText: this.app.i18n._('without contract'), allowBlank: true};

        Tine.Timetracker.TimeaccountContractFilterModel.superclass.initComponent.call(this);
    }
});

Tine.widgets.grid.FilterToolbar.FILTERS['timetracker.timeaccountcontract'] = Tine.Timetracker.TimeaccountContractFilterModel;/*
 * Tine 2.0
 * 
 * @package     Timetracker
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.namespace('Tine.Timetracker');

/**
 * Timeaccount grid panel
 * 
 * @namespace   Tine.Timetracker
 * @class       Tine.Timetracker.TimeaccountGridPanel
 * @extends     Tine.widgets.grid.GridPanel
 * 
 * <p>Timeaccount Grid Panel</p>
 * <p><pre>
 * TODO         copy action needs to copy the acl too
 * </pre></p>
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schuele <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2007-2009 Metaways Infosystems GmbH (http://www.metaways.de)
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Timetracker.TimeaccountGridPanel
 */
Tine.Timetracker.TimeaccountGridPanel = Ext.extend(Tine.widgets.grid.GridPanel, {
    // model generics
    recordClass: Tine.Timetracker.Model.Timeaccount,
    
    // grid specific
    defaultSortInfo: {field: 'creation_time', direction: 'DESC'},
    gridConfig: {
        autoExpandColumn: 'title'
    },
    copyEditAction: true,
    
    initComponent: function() {
        this.recordProxy = Tine.Timetracker.timeaccountBackend;
        
        this.actionToolbarItems = this.getToolbarItems();
        this.gridConfig.cm = this.getColumnModel();
        this.initFilterToolbar();
        
        this.plugins = this.plugins || [];
        this.plugins.push(this.filterToolbar);
        
        Tine.Timetracker.TimeaccountGridPanel.superclass.initComponent.call(this);
        
        this.action_addInNewWindow.setDisabled(! Tine.Tinebase.common.hasRight('manage', 'Timetracker', 'timeaccounts'));
        this.action_editInNewWindow.requiredGrant = 'editGrant';
    },
    
    /**
     * initialises filter toolbar
     * 
     * TODO created_by filter should be replaced by a 'responsible/organizer' filter like in tasks
     */
    initFilterToolbar: function() {
        this.filterToolbar = new Tine.widgets.grid.FilterToolbar({
            app: this.app,
            recordClass: this.recordClass,
            filterModels: Tine.Timetracker.Model.Timeaccount.getFilterModel(),
            defaultFilter: 'is_open',
            filters: [
                {field: 'is_open', operator: 'equals', value: true}
            ],
            plugins: [
                new Tine.widgets.grid.FilterToolbarQuickFilterPlugin()
            ]
        });
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
            {   id: 'tags', header: this.app.i18n._('Tags'), width: 50,  dataIndex: 'tags', sortable: false, renderer: Tine.Tinebase.common.tagsRenderer },
            {
                id: 'number',
                header: this.app.i18n._("Number"),
                width: 100,
                dataIndex: 'number'
            },{
                id: 'title',
                header: this.app.i18n._("Title"),
                width: 350,
                dataIndex: 'title'
            },{
                id: 'status',
                header: this.app.i18n._("Billed"),
                width: 150,
                dataIndex: 'status',
                renderer: this.statusRenderer.createDelegate(this)
            },{
                id: 'budget',
                header: this.app.i18n._("Budget"),
                width: 100,
                dataIndex: 'budget'
            },{
                id: 'billed_in',
                hidden: true,
                header: this.app.i18n._("Cleared in"),
                width: 150,
                dataIndex: 'billed_in'
            },{
                id: 'deadline',
                hidden: true,
                header: this.app.i18n._("Booking deadline"),
                width: 100,
                dataIndex: 'deadline'
            },{
                id: 'is_open',
                header: this.app.i18n._("Status"),
                width: 150,
                dataIndex: 'is_open',
                renderer: function(value) {
                    if(value) return this.app.i18n._('open');
                    return this.app.i18n._('closed');
                },
                scope: this,
                hidden: true
            }]
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
     * return additional tb items
     */
    getToolbarItems: function(){
        this.exportButton = new Ext.Action({
            text: _('Export'),
            iconCls: 'action_export',
            scope: this,
            requiredGrant: 'readGrant',
            disabled: true,
            allowMultiple: true,
            menu: {
                items: [
                    new Tine.widgets.grid.ExportButton({
                        text: this.app.i18n._('Export as ODS'),
                        format: 'ods',
                        exportFunction: 'Timetracker.exportTimeaccounts',
                        gridPanel: this
                    })
                    /*,
                    new Tine.widgets.grid.ExportButton({
                        text: this.app.i18n._('Export as CSV'),
                        format: 'csv',
                        exportFunction: 'Timetracker.exportTimesheets',
                        gridPanel: this
                    })
                    */
                ]
            }
        });
        
        return [
            Ext.apply(new Ext.Button(this.exportButton), {
                scale: 'medium',
                rowspan: 2,
                iconAlign: 'top'
            })
        ];
    }    
});
/**
 * Tine 2.0
 * 
 * @package     Timetracker
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.namespace('Tine.Timetracker');

Tine.Timetracker.TimeaccountEditDialog = Ext.extend(Tine.widgets.dialog.EditDialog, {
    
    /**
     * @private
     */
    windowNamePrefix: 'TimeaccountEditWindow_',
    appName: 'Timetracker',
    recordClass: Tine.Timetracker.Model.Timeaccount,
    recordProxy: Tine.Timetracker.timeaccountBackend,
    loadRecord: false,
    tbarItems: [{xtype: 'widget-activitiesaddbutton'}],
    
    /**
     * overwrite update toolbars function (we don't have record grants yet)
     */
    updateToolbars: function() {

    },
    
    onRecordLoad: function() {
        // make sure grants grid is initialised
        this.getGrantsGrid();
        
        var grants = this.record.get('grants') || [];
        this.grantsStore.loadData({results: grants});
        Tine.Timetracker.TimeaccountEditDialog.superclass.onRecordLoad.call(this);
        
    },
    
    onRecordUpdate: function() {
        Tine.Timetracker.TimeaccountEditDialog.superclass.onRecordUpdate.call(this);
        this.record.set('grants', '');
        
        var grants = [];
        this.grantsStore.each(function(_record){
            grants.push(_record.data);
        });
        
        this.record.set('grants', grants);
    },
    
    /**
     * returns dialog
     * 
     * NOTE: when this method gets called, all initalisation is done.
     */
    getFormItems: function() {
        
        var secondRow = [{
            fieldLabel: this.app.i18n._('Unit'),
            name: 'price_unit'
        }, {
            xtype: 'numberfield',
            fieldLabel: this.app.i18n._('Unit Price'),
            name: 'price',
            allowNegative: false
            //decimalSeparator: ','
        }, {
            fieldLabel: this.app.i18n._('Budget'),
            name: 'budget'
        }, {
            fieldLabel: this.app.i18n._('Status'),
            name: 'is_open',
            xtype: 'combo',
            mode: 'local',
            forceSelection: true,
            triggerAction: 'all',
            store: [[0, this.app.i18n._('closed')], [1, this.app.i18n._('open')]]
        }, {
            fieldLabel: this.app.i18n._('Billed'),
            name: 'status',
            xtype: 'combo',
            mode: 'local',
            forceSelection: true,
            triggerAction: 'all',
            value: 'not yet billed',
            store: [
                ['not yet billed', this.app.i18n._('not yet billed')], 
                ['to bill', this.app.i18n._('to bill')],
                ['billed', this.app.i18n._('billed')]
            ]
        }, {
            //disabled: true,
            //emptyText: this.app.i18n._('not cleared yet...'),
            fieldLabel: this.app.i18n._('Cleared In'),
            name: 'billed_in',
            xtype: 'textfield'
        }, {
            fieldLabel: this.app.i18n._('Booking deadline'),
            name: 'deadline',
            xtype: 'combo',
            mode: 'local',
            forceSelection: true,
            triggerAction: 'all',
            value: 'none',
            store: [
                ['none', this.app.i18n._('none')], 
                ['lastweek', this.app.i18n._('last week')]
            ]
        }];
        
        if(Tine.Tinebase.appMgr.get('Sales')) {
            secondRow.push({
                xtype: 'tinerelationpickercombo',
                fieldLabel: this.app.i18n._('Cost Center'),
                editDialog: this,
                allowBlank: true,
                app: 'Sales',
                recordClass: Tine.Sales.Model.CostCenter,
                relationType: 'COST_CENTER',
                relationDegree: 'sibling',
                modelUnique: true
            });
        }
        
        secondRow.push({
            hideLabel: true,
            boxLabel: this.app.i18n._('Timesheets are billable'),
            name: 'is_billable',
            xtype: 'checkbox',
            columnWidth: .666
        })
        
        return {
            xtype: 'tabpanel',
            border: false,
            plain:true,
            activeTab: 0,
            plugins: [{
                ptype : 'ux.tabpanelkeyplugin'
            }],
            items:[{
                title: this.app.i18n.ngettext('Time Account', 'Time Accounts', 1),
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
                        fieldLabel: this.app.i18n._('Number'),
                        name: 'number',
                        allowBlank: false
                        }, {
                        columnWidth: .666,
                        fieldLabel: this.app.i18n._('Title'),
                        name: 'title',
                        allowBlank: false
                        }], [{
                        columnWidth: 1,
                        fieldLabel: this.app.i18n._('Description'),
                        xtype: 'textarea',
                        name: 'description',
                        height: 150
                        }], secondRow] 
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
                    items: [/*new Ext.Panel({
                        // @todo generalise!
                        title: this.app.i18n._('Description'),
                        iconCls: 'descriptionIcon',
                        layout: 'form',
                        labelAlign: 'top',
                        border: false,
                        items: [{
                            style: 'margin-top: -4px; border 0px;',
                            labelSeparator: '',
                            xtype:'textarea',
                            name: 'description',
                            hideLabel: true,
                            grow: false,
                            preventScrollbars:false,
                            anchor:'100% 100%',
                            emptyText: this.app.i18n._('Enter description')                            
                        }]
                    }),*/
                    new Tine.widgets.activities.ActivitiesPanel({
                        app: 'Timetracker',
                        showAddNoteForm: false,
                        border: false,
                        bodyStyle: 'border:1px solid #B5B8C8;'
                    }),
                    new Tine.widgets.tags.TagPanel({
                        app: 'Timetracker',
                        border: false,
                        bodyStyle: 'border:1px solid #B5B8C8;'
                    })]
                }]
            },{
                title: this.app.i18n._('Access'),
                layout: 'fit',
                items: [this.getGrantsGrid()]
            }, new Tine.widgets.activities.ActivitiesTabPanel({
                app: this.appName,
                record_id: (! this.copyRecord) ? this.record.id : null,
                record_model: this.appName + '_Model_' + this.recordClass.getMeta('modelName')
            })]
        };
    },
    
    getGrantsGrid: function() {
        if (! this.grantsGrid) {
            this.grantsStore =  new Ext.data.JsonStore({
                root: 'results',
                totalProperty: 'totalcount',
                // use account_id here because that simplifies the adding of new records with the search comboboxes
                id: 'account_id',
                fields: Tine.Timetracker.Model.TimeaccountGrant
            });
            
            var columns = [
                new Ext.ux.grid.CheckColumn({
                    header: this.app.i18n._('Book Own'),
                    dataIndex: 'bookOwnGrant',
                    tooltip: _('The grant to add Timesheets to this Timeaccount'),
                    width: 55
                }),
                new Ext.ux.grid.CheckColumn({
                    header: this.app.i18n._('View All'),
                    tooltip: _('The grant to view Timesheets of other users'),
                    dataIndex: 'viewAllGrant',
                    width: 55
                }),
                new Ext.ux.grid.CheckColumn({
                    header: this.app.i18n._('Book All'),
                    tooltip: _('The grant to add Timesheets for other users'),
                    dataIndex: 'bookAllGrant',
                    width: 55
                }),
                new Ext.ux.grid.CheckColumn({
                    header:this.app.i18n. _('Manage Clearing'),
                    tooltip: _('The grant to manage clearing of Timesheets'),
                    dataIndex: 'manageBillableGrant',
                    width: 55
                }),
                new Ext.ux.grid.CheckColumn({
                    header:this.app.i18n. _('Export'),
                    tooltip: _('The grant to export Timesheets of Timeaccount'),
                    dataIndex: 'exportGrant',
                    width: 55
                }),
                new Ext.ux.grid.CheckColumn({
                    header: this.app.i18n._('Manage All'),
                    tooltip: _('Includes all other grants'),
                    dataIndex: 'adminGrant',
                    width: 55
                })
            ];
            
            this.grantsGrid = new Tine.widgets.account.PickerGridPanel({
                selectType: 'both',
                title:  this.app.i18n._('Permissions'),
                store: this.grantsStore,
                hasAccountPrefix: true,
                configColumns: columns,
                selectAnyone: false,
                selectTypeDefault: 'group',
                recordClass: Tine.Tinebase.Model.Grant
            });
        }
        return this.grantsGrid;
    }
});

/**
 * Timetracker Edit Popup
 */
Tine.Timetracker.TimeaccountEditDialog.openWindow = function (config) {
    var id = (config.record && config.record.id) ? config.record.id : 0;
    var window = Tine.WindowFactory.getWindow({
        width: 800,
        height: 500,
        name: Tine.Timetracker.TimeaccountEditDialog.prototype.windowNamePrefix + id,
        contentPanelConstructor: 'Tine.Timetracker.TimeaccountEditDialog',
        contentPanelConstructorConfig: config
    });
    return window;
};
﻿/*
 * Tine 2.0
 * 
 * @package     Timetracker
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.namespace('Tine.Timetracker');

/**
 * Timesheet grid panel
 * 
 * @namespace   Tine.Timetracker
 * @class       Tine.Timetracker.TimesheetGridPanel
 * @extends     Tine.widgets.grid.GridPanel
 * 
 * <p>Timesheet Grid Panel</p>
 * <p><pre>
 * </pre></p>
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Timetracker.TimesheetGridPanel
 */
Tine.Timetracker.TimesheetGridPanel = Ext.extend(Tine.widgets.grid.GridPanel, {
    /**
     * record class
     * @cfg {Tine.Timetracker.Model.Timesheet} recordClass
     */
    recordClass: Tine.Timetracker.Model.Timesheet,
    
    /**
     * @private grid cfg
     */
    defaultSortInfo: {field: 'start_date', direction: 'DESC'},
    gridConfig: {
        autoExpandColumn: 'description'
    },
    copyEditAction: true,
    multipleEdit: true,
    multipleEditRequiredRight: 'manage_timeaccounts',
    
    /**
     * @private
     */
    initComponent: function() {
        this.recordProxy = Tine.Timetracker.timesheetBackend;
                
        this.gridConfig.cm = this.getColumnModel();
        this.initFilterToolbar();
        this.initDetailsPanel();
        
        this.plugins = this.plugins || [];
        this.plugins.push(this.filterToolbar);
        
        // only eval grants in action updater if user does not have the right to manage timeaccounts
        this.evalGrants = ! Tine.Tinebase.common.hasRight('manage', 'Timetracker', 'timeaccounts');
        
        Tine.Timetracker.TimesheetGridPanel.superclass.initComponent.call(this);
    },
 
    /**
     * initialises filter toolbar
     * @private
     */
    initFilterToolbar: function() {
        this.filterToolbar = new Tine.widgets.grid.FilterPanel({
            app: this.app,
            recordClass: Tine.Timetracker.Model.Timesheet,
            allowSaving: true,
            filterModels: Tine.Timetracker.Model.Timesheet.getFilterModel().concat(this.getCustomfieldFilters()),
            defaultFilter: 'start_date',
            filters: [
                {field: 'start_date', operator: 'within', value: 'weekThis'},
                {field: 'account_id', operator: 'equals', value: Tine.Tinebase.registry.get('currentAccount')}
            ]
        });
    },    
    
    /**
     * returns cm
     * 
     * @return Ext.grid.ColumnModel
     * @private
     */
    getColumnModel: function(){
        var columns = [
            { id: 'tags',               header: this.app.i18n._('Tags'),                width: 50,  dataIndex: 'tags', sortable: false,
                renderer: Tine.Tinebase.common.tagsRenderer },
            { id: 'start_date',         header: this.app.i18n._("Date"),                width: 120, dataIndex: 'start_date',
                renderer: Tine.Tinebase.common.dateRenderer },
            { id: 'start_time',         header: this.app.i18n._("Start time"),          width: 100, dataIndex: 'start_time',            hidden: true,
                renderer: Tine.Tinebase.common.timeRenderer },
            { id: 'timeaccount_id',     header: this.app.i18n._('Time Account (Number - Title)'), width: 500, dataIndex: 'timeaccount_id',
                renderer: this.rendererTimeaccountId },
            { id: 'timeaccount_closed', header: this.app.i18n._("Time Account closed"), width: 100, dataIndex: 'timeaccount_closed',    hidden: true,
                renderer: this.rendererTimeaccountClosed },
            { id: 'description',        header: this.app.i18n._("Description"),         width: 400, dataIndex: 'description',           hidden: true },
            { id: 'is_billable',        header: this.app.i18n._("Billable"),            width: 100, dataIndex: 'is_billable_combined',
                renderer: Tine.Tinebase.common.booleanRenderer },
            { id: 'is_cleared',         header: this.app.i18n._("Cleared"),             width: 100, dataIndex: 'is_cleared_combined',   hidden: true,
                renderer: Tine.Tinebase.common.booleanRenderer },
            { id: 'billed_in',          header: this.app.i18n._("Cleared in"),          width: 150, dataIndex: 'billed_in',             hidden: true },
            { id: 'account_id',         header: this.app.i18n._("Account"),             width: 350, dataIndex: 'account_id',
                renderer: Tine.Tinebase.common.usernameRenderer },
            { id: 'duration',           header: this.app.i18n._("Duration"),            width: 150, dataIndex: 'duration',
                renderer: Tine.Tinebase.common.minutesRenderer }
        ].concat(this.getModlogColumns());
        
        return new Ext.grid.ColumnModel({
            defaults: {
                sortable: true,
                resizable: true
            },
            // add custom fields
            columns: columns.concat(this.getCustomfieldColumns())
        });
    },
    
    /**
     * timeaccount renderer -> returns timeaccount title
     * 
     * @param {Array} timeaccount
     * @return {String}
     */
    rendererTimeaccountId: function(timeaccount) {
        return new Tine.Timetracker.Model.Timeaccount(timeaccount).getTitle();
    },
    
    /**
     * is timeaccount closed -> returns yes/no if timeaccount is closed
     * 
     * @param {} a
     * @param {} b
     * @param {Tine.Timetracker.Model.Timesheet} record
     * @return {String}
     */
    rendererTimeaccountClosed: function(a, b, record) {
        var isopen = (record.data.timeaccount_id.is_open == '1');
        return Tine.Tinebase.common.booleanRenderer(!isopen);
    },

    /**
     * @private
     */
    initDetailsPanel: function() {
        this.detailsPanel = new Tine.widgets.grid.DetailsPanel({
            gridpanel: this,
            
            // use default Tpl for default and multi view
            defaultTpl: new Ext.XTemplate(
                '<div class="preview-panel-timesheet-nobreak">',
                    '<!-- Preview timeframe -->',           
                    '<div class="preview-panel preview-panel-timesheet-left">',
                        '<div class="bordercorner_1"></div>',
                        '<div class="bordercorner_2"></div>',
                        '<div class="bordercorner_3"></div>',
                        '<div class="bordercorner_4"></div>',
                        '<div class="preview-panel-declaration">' /*+ this.app.i18n._('timeframe')*/ + '</div>',
                        '<div class="preview-panel-timesheet-leftside preview-panel-left">',
                            '<span class="preview-panel-bold">',
                            /*'First Entry'*/'<br/>',
                            /*'Last Entry*/'<br/>',
                            /*'Duration*/'<br/>',
                            '<br/>',
                            '</span>',
                        '</div>',
                        '<div class="preview-panel-timesheet-rightside preview-panel-left">',
                            '<span class="preview-panel-nonbold">',
                            '<br/>',
                            '<br/>',
                            '<br/>',
                            '<br/>',
                            '</span>',
                        '</div>',
                    '</div>',
                    '<!-- Preview summary -->',
                    '<div class="preview-panel-timesheet-right">',
                        '<div class="bordercorner_gray_1"></div>',
                        '<div class="bordercorner_gray_2"></div>',
                        '<div class="bordercorner_gray_3"></div>',
                        '<div class="bordercorner_gray_4"></div>',
                        '<div class="preview-panel-declaration">'/* + this.app.i18n._('summary')*/ + '</div>',
                        '<div class="preview-panel-timesheet-leftside preview-panel-left">',
                            '<span class="preview-panel-bold">',
                            this.app.i18n._('Total Timesheets') + '<br/>',
                            this.app.i18n._('Billable Timesheets') + '<br/>',
                            this.app.i18n._('Total Time') + '<br/>',
                            this.app.i18n._('Time of Billable Timesheets') + '<br/>',
                            '</span>',
                        '</div>',
                        '<div class="preview-panel-timesheet-rightside preview-panel-left">',
                            '<span class="preview-panel-nonbold">',
                            '{count}<br/>',
                            '{countbillable}<br/>',
                            '{sum}<br/>',
                            '{sumbillable}<br/>',
                            '</span>',
                        '</div>',
                    '</div>',
                '</div>'            
            ),
            
            showDefault: function(body) {
                
                var data = {
                    count: this.gridpanel.store.proxy.jsonReader.jsonData.totalcount,
                    countbillable: (this.gridpanel.store.proxy.jsonReader.jsonData.totalcountbillable) ? this.gridpanel.store.proxy.jsonReader.jsonData.totalcountbillable : 0,
                    sum:  Tine.Tinebase.common.minutesRenderer(this.gridpanel.store.proxy.jsonReader.jsonData.totalsum),
                    sumbillable: Tine.Tinebase.common.minutesRenderer(this.gridpanel.store.proxy.jsonReader.jsonData.totalsumbillable)
                };
                
                this.defaultTpl.overwrite(body, data);
            },
            
            showMulti: function(sm, body) {
                
                var data = {
                    count: sm.getCount(),
                    countbillable: 0,
                    sum: 0,
                    sumbillable: 0
                };
                sm.each(function(record){
                    
                    data.sum = data.sum + parseInt(record.data.duration);
                    if (record.data.is_billable_combined == '1') {
                        data.countbillable++;
                        data.sumbillable = data.sumbillable + parseInt(record.data.duration);
                    }
                });
                data.sum = Tine.Tinebase.common.minutesRenderer(data.sum);
                data.sumbillable = Tine.Tinebase.common.minutesRenderer(data.sumbillable);
                
                this.defaultTpl.overwrite(body, data);
            },
            
            tpl: new Ext.XTemplate(
                '<div class="preview-panel-timesheet-nobreak">',    
                    '<!-- Preview beschreibung -->',
                    '<div class="preview-panel preview-panel-timesheet-left">',
                        '<div class="bordercorner_1"></div>',
                        '<div class="bordercorner_2"></div>',
                        '<div class="bordercorner_3"></div>',
                        '<div class="bordercorner_4"></div>',
                        '<div class="preview-panel-declaration">' /* + this.app.i18n._('Description') */ + '</div>',
                        '<div class="preview-panel-timesheet-description preview-panel-left" ext:qtip="{[this.encode(values.description)]}">',
                            '<span class="preview-panel-nonbold">',
                             '{[this.encode(values.description, "longtext")]}',
                            '<br/>',
                            '</span>',
                        '</div>',
                    '</div>',
                    '<!-- Preview detail-->',
                    '<div class="preview-panel-timesheet-right">',
                        '<div class="bordercorner_gray_1"></div>',
                        '<div class="bordercorner_gray_2"></div>',
                        '<div class="bordercorner_gray_3"></div>',
                        '<div class="bordercorner_gray_4"></div>',
                        '<div class="preview-panel-declaration">' /* + this.app.i18n._('Detail') */ + '</div>',
                        '<div class="preview-panel-timesheet-leftside preview-panel-left">',
                        // @todo add custom fields here
                        /*
                            '<span class="preview-panel-bold">',
                            'Ansprechpartner<br/>',
                            'Newsletter<br/>',
                            'Ticketnummer<br/>',
                            'Ticketsubjekt<br/>',
                            '</span>',
                        */
                        '</div>',
                        '<div class="preview-panel-timesheet-rightside preview-panel-left">',
                            '<span class="preview-panel-nonbold">',
                            '<br/>',
                            '<br/>',
                            '<br/>',
                            '<br/>',
                            '</span>',
                        '</div>',
                    '</div>',
                '</div>',{
                encode: function(value, type, prefix) {
                    if (value) {
                        if (type) {
                            switch (type) {
                                case 'longtext':
                                    value = Ext.util.Format.ellipsis(value, 150);
                                    break;
                                default:
                                    value += type;
                            }                           
                        } else {
                            value = Ext.util.Format.htmlEncode(value);
                        }
                        
                        var encoded = Ext.util.Format.htmlEncode(value);
                        encoded = Ext.util.Format.nl2br(encoded);
                        
                        return encoded;
                    } else {
                        return '';
                    }
                }
            })
        });
    },
    
    /**
     * @private
     */
    initActions: function() {
        this.actions_exportTimesheet = new Ext.Action({
            text: this.app.i18n._('Export Timesheets'),
            iconCls: 'action_export',
            scope: this,
            requiredGrant: 'exportGrant',
            disabled: true,
            allowMultiple: true,
            menu: {
                items: [
                    new Tine.widgets.grid.ExportButton({
                        text: this.app.i18n._('Export as ODS'),
                        format: 'ods',
                        iconCls: 'tinebase-action-export-ods',
                        exportFunction: 'Timetracker.exportTimesheets',
                        gridPanel: this
                    }),
                    new Tine.widgets.grid.ExportButton({
                        text: this.app.i18n._('Export as CSV'),
                        format: 'csv',
                        iconCls: 'tinebase-action-export-csv',
                        exportFunction: 'Timetracker.exportTimesheets',
                        gridPanel: this
                    }),
                    new Tine.widgets.grid.ExportButton({
                        text: this.app.i18n._('Export as ...'),
                        iconCls: 'tinebase-action-export-xls',
                        exportFunction: 'Timetracker.exportTimesheets',
                        showExportDialog: true,
                        gridPanel: this
                    })
                ]
            }
        });
        
        // register actions in updater
        this.actionUpdater.addActions([
            this.actions_exportTimesheet
        ]);
        
        Tine.Timetracker.TimesheetGridPanel.superclass.initActions.call(this);
    },
    
    /**
     * add custom items to action toolbar
     * 
     * @return {Object}
     */
    getActionToolbarItems: function() {
        return [
            Ext.apply(new Ext.Button(this.actions_exportTimesheet), {
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
            this.actions_exportTimesheet
//            '-', {
//            text: _('Mass Update'),
//            iconCls: 'action_edit',
//            disabled: !Tine.Tinebase.common.hasRight('manage', 'Timetracker', 'timeaccounts'),
//            scope: this,
//            menu: {
//                items: [
//                    '<b class="x-ux-menu-title">' + _('Update field:') + '</b>',
//                    {
//                        text: this.app.i18n._('Billable'),
//                        field: 'is_billable',
//                        scope: this,
//                        handler: this.onMassUpdate
//                    }, {
//                        text: this.app.i18n._('Cleared'),
//                        field: 'is_cleared',
//                        scope: this,
//                        handler: this.onMassUpdate
//                    }, {
//                        text: this.app.i18n._('Cleared in'),
//                        field: 'billed_in',
//                        scope: this,
//                        handler: this.onMassUpdate
//                    }
//                ]
//            }
//        }
        ];
        
        return items;
    }
});
/**
 * Tine 2.0
 * 
 * @package     Timetracker
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.namespace('Tine.Timetracker');

/**
 * Timetracker Edit Dialog
 */
Tine.Timetracker.TimesheetEditDialog = Ext.extend(Tine.widgets.dialog.EditDialog, {

    /**
     * @private
     */
    windowNamePrefix: 'TimesheetEditWindow_',
    appName: 'Timetracker',
    recordClass: Tine.Timetracker.Model.Timesheet,
    recordProxy: Tine.Timetracker.timesheetBackend,
    loadRecord: false,
    tbarItems: null,
    evalGrants: false,
    
    /**
     * overwrite update toolbars function (we don't have record grants yet)
     */
    updateToolbars: function(record) {
        this.onTimeaccountUpdate();
        Tine.Timetracker.TimesheetEditDialog.superclass.updateToolbars.call(this, record, 'timeaccount_id');
    },
    
    /**
     * this gets called when initializing and if a new timeaccount is chosen
     * 
     * @param {} field
     * @param {} timeaccount
     */
    onTimeaccountUpdate: function(field, timeaccount) {
        // check for manage_timeaccounts right
        var manageRight = Tine.Tinebase.common.hasRight('manage', 'Timetracker', 'timeaccounts');
        
        var notBillable = false;
        var notClearable = false;

        var grants = timeaccount ? timeaccount.get('account_grants') : (this.record.get('timeaccount_id') ? this.record.get('timeaccount_id').account_grants : {});
        
        if (grants) {
            this.getForm().findField('account_id').setDisabled(! (grants.bookAllGrant || grants.adminGrant || manageRight));
            notBillable = ! (grants.manageBillableGrant || grants.adminGrant || manageRight);
            notClearable = ! (grants.adminGrant || manageRight);
            this.getForm().findField('billed_in').setDisabled(! (grants.adminGrant || manageRight));
        }

        if (timeaccount) {
            notBillable = notBillable || timeaccount.data.is_billable == "0" || this.record.get('timeaccount_id').is_billable == "0";
            
            // clearable depends on timeaccount is_billable as well (changed by ps / 2009-09-01, behaviour was inconsistent)
            notClearable = notClearable || timeaccount.data.is_billable == "0" || this.record.get('timeaccount_id').is_billable == "0";
        }
        
        this.getForm().findField('is_billable').setDisabled(notBillable);
        this.getForm().findField('is_cleared').setDisabled(notClearable);
        
        if (this.record.id == 0 && timeaccount) {
            // set is_billable for new records according to the timeaccount setting
            this.getForm().findField('is_billable').setValue(timeaccount.data.is_billable);
        }
    },

    /**
     * this gets called when initializing and if cleared checkbox is changed
     * 
     * @param {} field
     * @param {} newValue
     * 
     * @todo    add prompt later?
     */
    onClearedUpdate: function(field, checked) {
        if (!this.useMultiple) {
            this.getForm().findField('billed_in').setDisabled(! checked);
        }
    },
    
    initComponent: function() {
        var addNoteButton = new Tine.widgets.activities.ActivitiesAddButton({});
        this.tbarItems = [addNoteButton];
        this.supr().initComponent.apply(this, arguments);
    },
    
    /**
     * overwrites the isValid method on multipleEdit
     */
    isMultipleValid: function() {
        var valid = true;
        var keys = ['timeaccount_id', 'description', 'account_id'];
        Ext.each(keys, function(key) {
            var field = this.getForm().findField(key);
            if(field.edited && ! field.validate()) {
                field.markInvalid();
                valid = false;
            }
        }, this);
        return valid;
    },
    
    /**
     * returns dialog
     * 
     * NOTE: when this method gets called, all initalisation is done.
     */
    getFormItems: function() {
        return {
            xtype: 'tabpanel',
            border: false,
            plain:true,
            activeTab: 0,
            plugins: [{
                ptype : 'ux.tabpanelkeyplugin'
            }],
            items:[
                {
                title: this.app.i18n.ngettext('Timesheet', 'Timesheets', 1),
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
                    items: [[Tine.widgets.form.RecordPickerManager.get('Timetracker', 'Timeaccount', {
                        columnWidth: 1,
                        fieldLabel: this.app.i18n.ngettext('Time Account', 'Time Accounts', 1),
                        emptyText: this.app.i18n._('Select Time Account...'),
                        allowBlank: false,
                        forceSelection: true,
                        name: 'timeaccount_id',
                        listeners: {
                            scope: this,
                            render: function(field){
                                if(!this.useMultiple) {
                                    field.focus(false, 250);
                                }
                            },
                            select: this.onTimeaccountUpdate
                        }
                    })], [{
                        fieldLabel: this.app.i18n._('Duration'),
                        name: 'duration',
                        allowBlank: false,
                        xtype: 'tinedurationspinner'
                        }, {
                        fieldLabel: this.app.i18n._('Date'),
                        name: 'start_date',
                        allowBlank: false,
                        xtype: 'datefield'
                        }, {
                        fieldLabel: this.app.i18n._('Start'),
                        emptyText: this.app.i18n._('not set'),
                        name: 'start_time',
                        xtype: 'timefield'
                    }], [{
                        columnWidth: 1,
                        fieldLabel: this.app.i18n._('Description'),
                        emptyText: this.app.i18n._('Enter description...'),
                        name: 'description',
                        allowBlank: false,
                        xtype: 'textarea',
                        height: 150
                    }], [new Tine.Addressbook.SearchCombo({
                        allowBlank: false,
                        forceSelection: true,
                        columnWidth: 1,
                        disabled: true,
                        useAccountRecord: true,
                        userOnly: true,
                        nameField: 'n_fileas',
                        fieldLabel: this.app.i18n._('Account'),
                        name: 'account_id'
                    }), {
                        columnWidth: .25,
                        disabled: (this.useMultiple) ? false : true,
                        boxLabel: this.app.i18n._('Billable'),
                        name: 'is_billable',
                        xtype: 'checkbox'
                    }, {
                        columnWidth: .25,
                        disabled: (this.useMultiple) ? false : true,
                        boxLabel: this.app.i18n._('Cleared'),
                        name: 'is_cleared',
                        xtype: 'checkbox',
                        listeners: {
                            scope: this,
                            check: this.onClearedUpdate
                        }                        
                    }, {
                        columnWidth: .5,
                        disabled: (this.useMultiple) ? false : true,
                        fieldLabel: this.app.i18n._('Cleared In'),
                        name: 'billed_in',
                        xtype: 'textfield'
                    }]] 
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
                            showAddNoteForm: false,
                            border: false,
                            bodyStyle: 'border:1px solid #B5B8C8;'
                        }),
                        new Tine.widgets.tags.TagPanel({
                            app: 'Timetracker',
                            border: false,
                            bodyStyle: 'border:1px solid #B5B8C8;'
                        })
                    ]
                }]
            }, new Tine.widgets.activities.ActivitiesTabPanel({
                app: this.appName,
                record_id: (! this.copyRecord) ? this.record.id : null,
                record_model: this.appName + '_Model_' + this.recordClass.getMeta('modelName')
            })]
        };
    },
    
    /**
     * show error if request fails
     * 
     * @param {} response
     * @param {} request
     * @private
     */
    onRequestFailed: function(response, request) {
        if (response.code && response.code == 902) {
            // deadline exception
            Ext.MessageBox.alert(
                this.app.i18n._('Failed'), 
                String.format(this.app.i18n._('Could not save {0}.'), this.i18nRecordName) 
                    + ' ( ' + this.app.i18n._('Booking deadline for this Timeaccount has been exceeded.') /* + ' ' + response.message  */ + ')'
            );
        } else {
            // call default exception handler
            Tine.Tinebase.ExceptionHandler.handleRequestException(response);
        }
        this.loadMask.hide();
    }
});

/**
 * Timetracker Edit Popup
 */
Tine.Timetracker.TimesheetEditDialog.openWindow = function (config) {
    var id = (config.record && config.record.id) ? config.record.id : 0;
    var window = Tine.WindowFactory.getWindow({
        width: 800,
        height: 500,
        name: Tine.Timetracker.TimesheetEditDialog.prototype.windowNamePrefix + id,
        contentPanelConstructor: 'Tine.Timetracker.TimesheetEditDialog',
        contentPanelConstructorConfig: config
    });
    return window;
};
