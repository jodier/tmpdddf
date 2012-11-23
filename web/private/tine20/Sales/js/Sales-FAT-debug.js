/*!
 * Tine 2.0 - Sales 
 * Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 * http://www.gnu.org/licenses/agpl.html AGPL Version 3
 */
/*
 * Tine 2.0
 * 
 * @package     Sales
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schuele <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2009 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.namespace('Tine.Sales', 'Tine.Sales.Model');

Tine.Sales.Model.ProductArray = Tine.Tinebase.Model.genericFields.concat([
    {name: 'id',            type: 'string'},
    {name: 'name',          type: 'string'},
    {name: 'description',   type: 'string'},
    {name: 'price',         type: 'float'},
    {name: 'manufacturer',  type: 'string'},
    {name: 'category',      type: 'string'},
    // tine 2.0 tags and notes
    {name: 'tags'},
    {name: 'notes'},
    // relations with other objects
    { name: 'relations'}
]);

/**
 * @namespace Tine.Sales.Model
 * @class Tine.Sales.Model.Product
 * @extends Tine.Tinebase.data.Record
 * 
 * Product Record Definition
 */ 
Tine.Sales.Model.Product = Tine.Tinebase.data.Record.create(Tine.Sales.Model.ProductArray, {
    appName: 'Sales',
    modelName: 'Product',
    idProperty: 'id',
    titleProperty: 'name',
    // ngettext('Product', 'Products', n);
    recordName: 'Product',
    recordsName: 'Products',
    containerProperty: 'container_id',
    // ngettext('record list', 'record lists', n);
    containerName: 'All Products',
    containersName: 'Products',
    getTitle: function() {
        return this.get('name') ? this.get('name') : false;
    }
});

/**
 * @namespace Tine.Sales.Model
 * 
 * get default data for a new product
 *  
 * @return {Object} default data
 * @static
 */ 
Tine.Sales.Model.Product.getDefaultData = function() {
    
    var data = {};
    return data;
};

/**
 * @namespace Tine.Sales.Model
 * 
 * get product filter
 *  
 * @return {Array} filter objects
 * @static
 */ 
Tine.Sales.Model.Product.getFilterModel = function() {
    var app = Tine.Tinebase.appMgr.get('Sales');
    
    return [
        {label: _('Quick search'), field: 'query', operators: ['contains']},
        {label: app.i18n._('Product name'),   field: 'name' },
        {filtertype: 'tinebase.tag', app: app},
        {label: app.i18n._('Creator'), field: 'created_by', valueType: 'user'}
    ];
};


// Contract model fields
Tine.Sales.Model.ContractArray = Tine.Tinebase.Model.genericFields.concat([
    // contract only fields
    { name: 'id' },
    { name: 'number' },
    { name: 'title' },
    { name: 'description' },
    { name: 'status' },
    { name: 'cleared' },
    { name: 'cleared_in' },
    // tine 2.0 notes field
    { name: 'notes'},
    // linked contacts
    { name: 'relations' }
]);

/**
 * @namespace Tine.Sales.Model
 * @class Tine.Sales.Model.Contract
 * @extends Tine.Tinebase.data.Record
 * 
 * Contract Record Definition
 */ 
Tine.Sales.Model.Contract = Tine.Tinebase.data.Record.create(Tine.Sales.Model.ContractArray, {
    appName: 'Sales',
    modelName: 'Contract',
    idProperty: 'id',
    titleProperty: 'title',
    // ngettext('Contract', 'Contracts', n);
    recordName: 'Contract',
    recordsName: 'Contracts',
    containerProperty: 'container_id',
    // ngettext('All Contracts', 'contracts lists', n);
    containerName: 'All Contracts',
    containersName: 'contracts lists',
    getTitle: function() {
        return this.get('number')  + ' - ' + this.get('title');
    }
});

/**
 * @namespace Tine.Sales.Model
 * 
 * get default data for a new Contract
 *  
 * @return {Object} default data
 * @static
 */
Tine.Sales.Model.Contract.getDefaultData = function() {
    return {
        container_id: Tine.Sales.registry.get('defaultContainer')
    };
};

Tine.Sales.Model.Contract.getFilterModel = function() {
    var app = Tine.Tinebase.appMgr.get('Sales');
    
    return [
        {label: _('Quick search'), field: 'query', operators: ['contains']},
        {label: app.i18n._('Contract name'),   field: 'name' },
        {label: app.i18n._('Creator'), field: 'created_by', valueType: 'user'},
        {
            label: app.i18n._('Status'),
            field: 'status',
            filtertype: 'tine.widget.keyfield.filter', 
            app: app, 
            keyfieldName: 'contractStatus'
        },
        {
            label: app.i18n._('Cleared'),
            field: 'cleared',
            filtertype: 'tine.widget.keyfield.filter', 
            app: app, 
            keyfieldName: 'contractCleared'
        },
        {label: app.i18n._('Cleared in'), field: 'cleared_in' },
        {filtertype: 'tinebase.tag', app: app}
        
    ];
};

// COST CENTER
Tine.Sales.Model.CostCenterArray = [
    { name: 'id' },
    { name: 'number' },
    { name: 'remark' },
    { name: 'relations' }
];

/**
 * @namespace Tine.Sales.Model
 * @class Tine.Sales.Model.CostCenter
 * @extends Tine.Tinebase.data.Record
 *
 * CostCenter Record Definition
 */
Tine.Sales.Model.CostCenter = Tine.Tinebase.data.Record.create(Tine.Sales.Model.CostCenterArray, {
    appName: 'Sales',
    modelName: 'CostCenter',
    idProperty: 'id',
    titleProperty: 'remark',
    // ngettext('CostCenter', 'CostCenters', n);
    recordName: 'Cost Center',
    recordsName: 'Cost Centers',
    // ngettext('All CostCenters', 'All CostCenters', n);
    containerName: 'All CostCenters',
    containersName: 'All CostCenters'
});

// costcenters filtermodel
Tine.Sales.Model.CostCenter.getFilterModel = function() {
    var app = Tine.Tinebase.appMgr.get('Sales');
    
    return [
        {label: _('Quick search'), field: 'query', operators: ['contains']},
        {label: app.i18n._('Number'), field: 'number' },
        {label: app.i18n._('Remark'), field: 'remark' }
    ];
};
// DIVISION

Tine.Sales.Model.DivisionArray = Tine.Tinebase.Model.genericFields.concat([
    {name: 'id',            type: 'string'},
    {name: 'title',         type: 'string'}
]);

/**
 * @namespace Tine.Sales.Model
 * @class Tine.Sales.Model.Division
 * @extends Tine.Tinebase.data.Record
 * 
 * Division Record Definition
 */ 
Tine.Sales.Model.Division = Tine.Tinebase.data.Record.create(Tine.Sales.Model.DivisionArray, {
    appName: 'Sales',
    modelName: 'Division',
    idProperty: 'id',
    titleProperty: 'title',
    // ngettext('Division', 'Divisions', n);
    recordName: 'Division',
    recordsName: 'Divisions',
    // ngettext('record list', 'record lists', n);
    containerName: 'All Divisions',
    containersName: 'Divisions'
});

// divisions filtermodel
Tine.Sales.Model.Division.getFilterModel = function() {
    var app = Tine.Tinebase.appMgr.get('Sales');
    
    return [
        {label: _('Quick search'), field: 'query', operators: ['contains']},
        {label: app.i18n._('Title'), field: 'title' }
    ];
};/*
 * Tine 2.0
 * 
 * @package     Sales
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schuele <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2007-2009 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.namespace('Tine.Sales');

/**
 * @namespace Tine.Sales
 * @class Tine.Sales.MainScreen
 * @extends Tine.widgets.MainScreen
 * MainScreen of the Sales Application <br>
 * <pre>
 * TODO         generalize this
 * </pre>
 * 
 * @author      Philipp Schuele <p.schuele@metaways.de>
 * 
 * @constructor
 * Constructs mainscreen of the Sales application
 */
Tine.Sales.MainScreen = Ext.extend(Tine.widgets.MainScreen, {
    activeContentType: 'Product',
    contentTypes: [
        {model: 'Product',  requiredRight: null, singularContainerMode: true},
        {model: 'Contract', requiredRight: null, singularContainerMode: true, genericCtxActions: ['grants']}]
});

/**
 * @namespace Tine.Sales
 * @class Tine.Sales.FilterPanel
 * @extends Tine.widgets.persistentfilter.PickerPanel
 * Sales Filter Panel<br>
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Alexander Stintzing <a.stintzing@metaways.de>
 * @copyright   Copyright (c) 2007-2012 Metaways Infosystems GmbH (http://www.metaways.de)
 */
Tine.Sales.ProductFilterPanel = function(config) {
    Ext.apply(this, config);
    Tine.Sales.ProductFilterPanel.superclass.constructor.call(this);
};

Ext.extend(Tine.Sales.ProductFilterPanel, Tine.widgets.persistentfilter.PickerPanel, {
    filter: [{field: 'model', operator: 'equals', value: 'Sales_Model_ProductFilter'}]
});

Tine.Sales.ContractFilterPanel = function(config) {
    Ext.apply(this, config);
    Tine.Sales.ContractFilterPanel.superclass.constructor.call(this);
};

Ext.extend(Tine.Sales.ContractFilterPanel, Tine.widgets.persistentfilter.PickerPanel, {
    filter: [{field: 'model', operator: 'equals', value: 'Sales_Model_ContractFilter'}]
});

/**
 * default contracts backend
 */
Tine.Sales.contractBackend = new Tine.Tinebase.data.RecordProxy({
    appName: 'Sales',
    modelName: 'Contract',
    recordClass: Tine.Sales.Model.Contract
});

/**
 * @namespace Tine.Sales
 * @class Tine.Sales.productBackend
 * @extends Tine.Tinebase.data.RecordProxy
 * 
 * Product Backend
 */ 
Tine.Sales.productBackend = new Tine.Tinebase.data.RecordProxy({
    appName: 'Sales',
    modelName: 'Product',
    recordClass: Tine.Sales.Model.Product
});

/**
 * default costcenter backend
 */
Tine.Sales.costcenterBackend = new Tine.Tinebase.data.RecordProxy({
    appName: 'Sales',
    modelName: 'CostCenter',
    recordClass: Tine.Sales.Model.CostCenter
});

/**
 * default division backend
 */
Tine.Sales.divisionBackend = new Tine.Tinebase.data.RecordProxy({
    appName: 'Sales',
    modelName: 'Division',
    recordClass: Tine.Sales.Model.Division
});/*
 * Tine 2.0
 * 
 * @package     Sales
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Alexander Stintzing <a.stintzing@metaways.de>
 * @copyright   Copyright (c) 2009-2012 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */

Ext.ns('Tine.Sales');

/**
 * @namespace   Tine.Sales
 * @class       Tine.Sales.AdminPanel
 * @extends     Ext.FormPanel
 * @author      Alexander Stintzing <alex@stintzing.net>
 */
Tine.Sales.AdminPanel = Ext.extend(Ext.FormPanel, {
    appName : 'Sales',

    layout : 'fit',
    border : false,
    cls : 'tw-editdialog',    

    labelAlign : 'top',

    anchor : '100% 100%',
    deferredRender : false,
    buttonAlign : null,
    bufferResize : 500,

    /**
     * init component
     */
    initComponent: function() {

        if (!this.app) {
            this.app = Tine.Tinebase.appMgr.get(this.appName);
        }

        Tine.log.debug('initComponent: appName: ', this.appName);
        Tine.log.debug('initComponent: app: ', this.app);

        // init actions
        this.initActions();
        // init buttons and tbar
        this.initButtons();

        // get items for this dialog
        this.items = this.getFormItems();
        
        Tine.Sales.AdminPanel.superclass.initComponent.call(this);
    },

    /**
     * init actions
     */
    initActions: function() {
        this.action_cancel = new Ext.Action({
            text : this.app.i18n._('Cancel'),
            minWidth : 70,
            scope : this,
            handler : this.onCancel,
            iconCls : 'action_cancel'
        });

        this.action_update = new Ext.Action({
            text : this.app.i18n._('OK'),
            minWidth : 70,
            scope : this,
            handler : this.onUpdate,
            iconCls : 'action_saveAndClose'
        });
    },

    /**
     * init buttons
     */
    initButtons : function() {
        this.fbar = [ '->', this.action_cancel, this.action_update ];
    },  

    /**
     * is called when the component is rendered
     * @param {} ct
     * @param {} position
     */
    onRender : function(ct, position) {
        this.loadMask = new Ext.LoadMask(ct, {msg: _('Loading...')});
        Tine.Sales.AdminPanel.superclass.onRender.call(this, ct, position);

        // generalized keybord map for edit dlgs
        var map = new Ext.KeyMap(this.el, [ {
            key : [ 10, 13 ], // ctrl + return
            ctrl : true,
            fn : this.onUpdate,
            scope : this
        } ]);

    },
    
    /**
     * closes the window
     */
    onCancel: function() {
        this.fireEvent('cancel');
        this.purgeListeners();
        this.window.close();
    },

    /**
     * save record and close window
     */
    onUpdate : function() {
        Ext.Ajax.request({
            url: 'index.php',
            scope: this,
            params: {
                method: 'Sales.setConfig',
                config: {
                    contractNumberGeneration: this.getForm().findField('contractNumberGeneration').getValue(),
                    contractNumberValidation: this.getForm().findField('contractNumberValidation').getValue()
                }
            },
            success : function(_result, _request) {
                this.onCancel();
            }
        });
    },

    /**
     * create and return form items
     * @return Object
     */
    getFormItems: function() {
        
        var cng = Tine.Sales.registry.get('config').contractNumberGeneration,
            cnv = Tine.Sales.registry.get('config').contractNumberValidation;

        return {
            border: false,
            frame:  false,
            layout: 'border',

            items: [{
                region: 'center',
                border: false,
                frame:  false,
                layout : {
                    align: 'stretch',
                    type:  'vbox'
                    },
                items: [{
                    layout:  'form',
                    margins: '10px 10px',
                    border:  false,
                    frame:   false,
                    items: [
                        {
                            fieldLabel: this.app.i18n._(cng.definition.label),
                            name: 'contractNumberGeneration',
                            xtype: 'combo',
                            mode: 'local',
                            forceSelection: true,
                            triggerAction: 'all',
                            value: cng.value ? cng.value : cng['default'],
                            store: cng.definition.options
                        },
                        {
                            fieldLabel: this.app.i18n._(cnv.definition.label),
                            name: 'contractNumberValidation',
                            xtype: 'combo',
                            mode: 'local',
                            value: cnv.value ? cnv.value : cnv['default'],
                            forceSelection: true,
                            triggerAction: 'all',
                            store: cnv.definition.options
                        }
                    ] 
                    }]

            }]
        };
    } 
});

Tine.Sales.AdminPanel.openWindow = function(config) {
    var window = Tine.WindowFactory.getWindow({
        modal: true,
        width : 240,
        height : 250,
        contentPanelConstructor : 'Tine.Sales.AdminPanel',
        contentPanelConstructorConfig : config
    });
    return window;
};/*
 * Tine 2.0
 * Sales combo box and store
 * 
 * @package     Sales
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Alexander Stintzing <a.stintzing@metaways.de>
 * @copyright   Copyright (c) 2012 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */

Ext.ns('Tine.Sales');

/**
 * Contract selection combo box
 * 
 * @namespace   Tine.Sales
 * @class       Tine.Sales.ContractSearchCombo
 * @extends     Ext.form.ComboBox
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Alexander Stintzing <a.stintzing@metaways.de>
 * @copyright   Copyright (c) 2012 Metaways Infosystems GmbH (http://www.metaways.de)
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Sales.ContractSearchCombo
 */
Tine.Sales.ContractSearchCombo = Ext.extend(Tine.Tinebase.widgets.form.RecordPickerComboBox, {
    
    allowBlank: false,
    itemSelector: 'div.search-item',
    minListWidth: 200,
    
    //private
    initComponent: function(){
        this.recordClass = Tine.Sales.Model.Contract;
        this.recordProxy = Tine.Sales.contractBackend;
        
        this.initTemplate();
        Tine.Sales.ContractSearchCombo.superclass.initComponent.call(this);
    },
    
    /**
     * init template
     * @private
     */
    initTemplate: function() {
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
                        var ret = '';
                        if(values.number) ret += '<b>' + Ext.util.Format.htmlEncode(values.number) + '</b> - ';
                        ret += Ext.util.Format.htmlEncode(values.title);
                        return ret;
                        
                    }
                }
            );
        }
    }
});

Tine.widgets.form.RecordPickerManager.register('Sales', 'Contract', Tine.Sales.ContractSearchCombo);
/*
 * Tine 2.0
 * Sales combo box and store
 * 
 * @package     Sales
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Alexander Stintzing <a.stintzing@metaways.de>
 * @copyright   Copyright (c) 2012 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */

Ext.ns('Tine.Sales');

/**
 * CostCenter selection combo box
 * 
 * @namespace   Tine.Sales
 * @class       Tine.Sales.CostCenterSearchCombo
 * @extends     Ext.form.ComboBox
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Alexander Stintzing <a.stintzing@metaways.de>
 * @copyright   Copyright (c) 2012 Metaways Infosystems GmbH (http://www.metaways.de)
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Sales.CostCenterSearchCombo
 */
Tine.Sales.CostCenterSearchCombo = Ext.extend(Tine.Tinebase.widgets.form.RecordPickerComboBox, {
    
    allowBlank: false,
    itemSelector: 'div.search-item',
    minListWidth: 200,
    sortBy: 'number',
    
    //private
    initComponent: function(){
        this.recordClass = Tine.Sales.Model.CostCenter;
        this.recordProxy = Tine.Sales.costcenterBackend;
        this.initTemplate();
        
        Tine.Sales.CostCenterSearchCombo.superclass.initComponent.call(this);
    },
    
    /**
     * init template
     * @private
     */
    initTemplate: function() {
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
                        var ret = '';
                        if(values.number) ret += '<b>' + Ext.util.Format.htmlEncode(values.number) + '</b> - ';
                        ret += Ext.util.Format.htmlEncode(values.remark);
                        return ret;
                        
                    }
                }
            );
        }
    }
});

Tine.widgets.form.RecordPickerManager.register('Sales', 'CostCenter', Tine.Sales.CostCenterSearchCombo);
/*
 * Tine 2.0
 * 
 * @package     Sales
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.namespace('Tine.Sales');

/**
 * Contract grid panel
 * 
 * @namespace   Tine.Sales
 * @class       Tine.Sales.ContractGridPanel
 * @extends     Tine.widgets.grid.GridPanel
 * 
 * <p>Contract Grid Panel</p>
 * <p><pre>
 * </pre></p>
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schuele <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2007-2010 Metaways Infosystems GmbH (http://www.metaways.de)
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Sales.ContractGridPanel
 */
Tine.Sales.ContractGridPanel = Ext.extend(Tine.widgets.grid.GridPanel, {
    // model generics
    recordClass: Tine.Sales.Model.Contract,
    
    // grid specific
    defaultSortInfo: {field: 'title', dir: 'ASC'},
    gridConfig: {
        autoExpandColumn: 'title'
    },
    
    multipleEdit: true,
    
    initComponent: function() {
        this.recordProxy = Tine.Sales.contractBackend;
        
        this.gridConfig.columns = this.getColumns();
        this.initFilterToolbar();
        this.plugins.push(this.filterToolbar);
        
        Tine.Sales.ContractGridPanel.superclass.initComponent.call(this);
        this.action_addInNewWindow.actionUpdater = function() {
            var defaultContainer = this.app.getRegistry().get('defaultContainer');
            this.action_addInNewWindow.setDisabled(! defaultContainer.account_grants[this.action_addInNewWindow.initialConfig.requiredGrant]);
        }
    },
    
    /**
     * initialises filter toolbar
     */
    initFilterToolbar: function() {
        this.filterToolbar = new Tine.widgets.grid.FilterToolbar({
            recordClass: this.recordClass,
            filterModels: Tine.Sales.Model.Contract.getFilterModel(),
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
     * 
     * @todo    add more columns
     */
    getColumns: function(){
        return [{
            id: 'number',
            header: this.app.i18n._("Contract number"),
            width: 100,
            sortable: true,
            dataIndex: 'number'
        },{
            id: 'title',
            header: this.app.i18n._("Title"),
            width: 200,
            sortable: true,
            dataIndex: 'title'
        },{
            id: 'status',
            header: this.app.i18n._("Status"),
            width: 100,
            sortable: true,
            dataIndex: 'status',
            renderer: Tine.Tinebase.widgets.keyfield.Renderer.get('Sales', 'contractStatus')
        },{
            id: 'cleared',
            header: this.app.i18n._("Cleared"),
            width: 15,
            sortable: true,
            dataIndex: 'cleared',
            renderer: Tine.Tinebase.widgets.keyfield.Renderer.get('Sales', 'contractCleared')
        },{
            id: 'cleared_in',
            header: this.app.i18n._("Cleared in"),
            width: 100,
            sortable: true,
            dataIndex: 'cleared_in'
        }
        ];
    }
});
/*
 * Tine 2.0
 * 
 * @package     Sales
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schuele <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2007-2010 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.namespace('Tine.Sales');

/**
 * Contract edit dialog
 * 
 * @namespace   Tine.Sales
 * @class       Tine.Sales.ContractEditDialog
 * @extends     Tine.widgets.dialog.EditDialog
 * 
 * <p>Contract Edit Dialog</p>
 * <p><pre>
 * </pre></p>
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schuele <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2007-2010 Metaways Infosystems GmbH (http://www.metaways.de)
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Sales.ContractGridPanel
 */
Tine.Sales.ContractEditDialog = Ext.extend(Tine.widgets.dialog.EditDialog, {
    /**
     * @private
     */
    labelAlign: 'side',
    
    /**
     * @private
     */
    windowNamePrefix: 'ContractEditWindow_',
    appName: 'Sales',
    recordClass: Tine.Sales.Model.Contract,
    recordProxy: Tine.Sales.contractBackend,
    tbarItems: [{xtype: 'widget-activitiesaddbutton'}],
    /**
     * if true, number will be readOnly and will be generated automatically
     * @type {Boolean} autoGenerateNumber
     */
    autoGenerateNumber: null,
    /**
     * how should the number be validated text/integer possible
     * @type {String} validateNumber
     */
    validateNumber: null,
    
    initComponent: function() {
        this.autoGenerateNumber = (Tine.Sales.registry.get('config').contractNumberGeneration.value == 'auto') ? true : false;
        this.validateNumber = Tine.Sales.registry.get('config').contractNumberValidation.value;
        Tine.Sales.ContractEditDialog.superclass.initComponent.call(this);
    },
    
    /**
     * reqests all data needed in this dialog
     */
    requestData: function() {
        this.loadRequest = Ext.Ajax.request({
            scope: this,
            success: function(response) {
                this.record = this.recordProxy.recordReader(response);
                this.onRecordLoad();
            },
            params: {
                method: 'Sales.getContract',
                id: this.record.id
            }
        });
    },
    
    /**
     * called on multiple edit
     * @return {Boolean}
     */
    isMultipleValid: function() {
        return true;
    },
    
    /**
     * extra validation for the number field, calls parent
     * @return {Boolean}
     */
    isValid: function() {
        var valid = Tine.Sales.ContractEditDialog.superclass.isValid.call(this);
        var isValid = this.autoGenerateNumber ? true : (this.validateNumber == 'integer') ? Ext.isNumber(Ext.num(this.getForm().findField('number').getValue())) : true;
        if(!isValid) {
            this.getForm().findField('number').markInvalid(this.app.i18n._('Please use a decimal number here!'));
        }
        return isValid && valid;
    },
    
    /**
     * returns dialog
     * 
     * NOTE: when this method gets called, all initalisation is done.
     */
    getFormItems: function() {
        return {
            xtype: 'tabpanel',
            layoutOnTabChange: true,
            border: false,
            plain:true,
            activeTab: 0,
            border: false,
            plugins: [{
                ptype : 'ux.tabpanelkeyplugin'
            }],
            items:[
                {
                title: this.app.i18n.n_('Contract', 'Contract', 1),
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
                        columnWidth: .25,
                        fieldLabel: this.app.i18n._('Number'),
                        name: 'number',
                        multiEditable: false,
                        readOnly: this.autoGenerateNumber,
                        allowBlank: this.autoGenerateNumber
                    },{
                        columnWidth: .75,
                        fieldLabel: this.app.i18n._('Title'),
                        name: 'title',
                        allowBlank: false
                    }], [{
                            columnWidth: .5,
                            xtype: 'tinerelationpickercombo',
                            fieldLabel: this.app.i18n._('Contact Customer'),
                            editDialog: this,
                            allowBlank: true,
                            app: 'Addressbook',
                            recordClass: Tine.Addressbook.Model.Contact,
                            relationType: 'CUSTOMER',
                            relationDegree: 'sibling',
                            modelUnique: true
                        }, {
                            columnWidth: .5,
                            editDialog: this,
                            xtype: 'tinerelationpickercombo',
                            fieldLabel: this.app.i18n._('Contact Responsible'),
                            allowBlank: true,
                            app: 'Addressbook',
                            recordClass: Tine.Addressbook.Model.Contact,
                            relationType: 'RESPONSIBLE',
                            relationDegree: 'sibling',
                            modelUnique: true
                        }/*, {
                            name: 'customer',
                            fieldLabel: this.app.i18n._('Company')
                        }*/],[
                        
                        new Tine.Tinebase.widgets.keyfield.ComboBox({
                                    app: 'Sales',
                                    keyFieldName: 'contractStatus',
                                    fieldLabel: this.app.i18n._('Status'),
                                    name: 'status'
                                }),
                                
                        new Tine.Tinebase.widgets.keyfield.ComboBox({
                                    app: 'Sales',
                                    keyFieldName: 'contractCleared',
                                    fieldLabel: this.app.i18n._('Cleared'),
                                    name: 'cleared'
                                }),
                        {
                            fieldLabel: this.app.i18n._('Cleared in'),
                            name: 'cleared_in',
                            xtype: 'textfield'
                        }
                    ], [{
                            columnWidth: 1,
                            fieldLabel: this.app.i18n._('Description'),
                            emptyText: this.app.i18n._('Enter description...'),
                            name: 'description',
                            xtype: 'textarea',
                            height: 200
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
                            app: 'Sales',
                            showAddNoteForm: false,
                            border: false,
                            bodyStyle: 'border:1px solid #B5B8C8;'
                        }),
                        new Tine.widgets.tags.TagPanel({
                            app: 'Sales',
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
 * Sales Edit Popup
 */
Tine.Sales.ContractEditDialog.openWindow = function (config) {
    var window = Tine.WindowFactory.getWindow({
        width: 800,
        height: 470,
        name: Tine.Sales.ContractEditDialog.prototype.windowNamePrefix + Ext.id(),
        contentPanelConstructor: 'Tine.Sales.ContractEditDialog',
        contentPanelConstructorConfig: config
    });
    return window;
};
/*
 * Tine 2.0
 * 
 * @package     Sales
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2009-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.namespace('Tine.Sales');

/**
 * Product grid panel
 * 
 * @namespace   Tine.Sales
 * @class       Tine.Sales.ProductGridPanel
 * @extends     Tine.widgets.grid.GridPanel
 * 
 * <p>Product Grid Panel</p>
 * <p><pre>
 * </pre></p>
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Sales.ProductGridPanel
 */
Tine.Sales.ProductGridPanel = Ext.extend(Tine.widgets.grid.GridPanel, {
    /**
     * record class
     * @cfg {Tine.Sales.Model.Product} recordClass
     */
    recordClass: Tine.Sales.Model.Product,
    
    /**
     * eval grants
     * @cfg {Boolean} evalGrants
     */
    evalGrants: false,
    
    /**
     * grid specific
     * @private
     */
    defaultSortInfo: {field: 'name', direction: 'DESC'},
    gridConfig: {
        autoExpandColumn: 'name'
    },
     
    /**
     * inits this cmp
     * @private
     */
    initComponent: function() {
        this.recordProxy = Tine.Sales.productBackend;
        
        this.actionToolbarItems = this.getToolbarItems();
        this.contextMenuItems = [
        ];

        this.gridConfig.cm = this.getColumnModel();
        this.filterToolbar = this.getFilterToolbar();
        
        this.plugins = this.plugins || [];
        this.plugins.push(this.filterToolbar);
        
        Tine.Sales.ProductGridPanel.superclass.initComponent.call(this);
        
        // actions depend on manage_products right
        this.selectionModel.on('selectionchange', function(sm) {
            var hasManageRight = Tine.Tinebase.common.hasRight('manage', 'Sales', 'products');

            if (hasManageRight) {
                Tine.widgets.actionUpdater(sm, this.actions, this.recordClass.getMeta('containerProperty'), !this.evalGrants);
                if (this.updateOnSelectionChange && this.detailsPanel) {
                    this.detailsPanel.onDetailsUpdate(sm);
                }
            } else {
                this.action_editInNewWindow.setDisabled(true);
                this.action_deleteRecord.setDisabled(true);
                this.action_tagsMassAttach.setDisabled(true);
            }
        }, this);

        this.action_addInNewWindow.setDisabled(! Tine.Tinebase.common.hasRight('manage', 'Sales', 'products'));
    },
    
    /**
     * initialises filter toolbar
     * 
     * @return Tine.widgets.grid.FilterToolbar
     * @private
     */
    getFilterToolbar: function() {
        return new Tine.widgets.grid.FilterToolbar({
            filterModels: Tine.Sales.Model.Product.getFilterModel(),
            defaultFilter: 'query',
            recordClass: this.recordClass,
            filters: [],
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
                sortable: true
            },
            columns: [
                {header: this.app.i18n._('Tags'), id: 'tags', dataIndex: 'tags', width: 50, renderer: Tine.Tinebase.common.tagsRenderer, sortable: false},
                {header: this.app.i18n._('Name'), id: 'name', dataIndex: 'name', width: 200},
                {header: this.app.i18n._('Manufacturer'), id: 'manufacturer', dataIndex: 'manufacturer', width: 100},
                {header: this.app.i18n._('Category'), id: 'category', dataIndex: 'category', width: 100},
                {header: this.app.i18n._('Description'), id: 'description', dataIndex: 'description', width: 150, sortable: false, hidden: true},
                {header: this.app.i18n._('Price'), id: 'price', dataIndex: 'price', width: 75, renderer: Ext.util.Format.euMoney}
            ]
        });
    },

    /**
     * return additional tb items
     * @private
     */
    getToolbarItems: function(){
        
        return [
        ];
    }    
});
/*
 * Tine 2.0
 * 
 * @package     Sales
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schuele <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2009 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.namespace('Tine.Sales');

/**
 * Product edit dialog
 * 
 * @namespace   Tine.Sales
 * @class       Tine.Sales.ProductEditDialog
 * @extends     Tine.widgets.dialog.EditDialog
 * 
 * <p>Product Edit Dialog</p>
 * <p><pre>
 * TODO         make category a combobox + get data from settings
 * </pre></p>
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schuele <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2009 Metaways Infosystems GmbH (http://www.metaways.de)
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Sales.ProductGridPanel
 */
Tine.Sales.ProductEditDialog = Ext.extend(Tine.widgets.dialog.EditDialog, {
    /**
     * @private
     */
    windowNamePrefix: 'ProductEditWindow_',
    appName: 'Sales',
    recordClass: Tine.Sales.Model.Product,
    recordProxy: Tine.Sales.productBackend,
    tbarItems: [{xtype: 'widget-activitiesaddbutton'}],
    evalGrants: false,
    
    /**
     * @private
     */
    initComponent: function() {
        Tine.Sales.ProductEditDialog.superclass.initComponent.call(this);
    },
    
    /**
     * executed when record is loaded
     * @private
     */
    onRecordLoad: function() {
        Tine.Sales.ProductEditDialog.superclass.onRecordLoad.call(this);
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
            border: false,
            plugins: [{
                ptype : 'ux.tabpanelkeyplugin'
            }],
            items:[
                {
                title: this.app.i18n.n_('Product', 'Product', 1),
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
                        fieldLabel: this.app.i18n._('Name'),
                        name: 'name',
                        allowBlank: false
                    }], [{
                        columnWidth: 1,
                        xtype: 'numberfield',
                        fieldLabel: this.app.i18n._('Price'),
                        name: 'price',
                        allowNegative: false,
                        allowBlank: false
                        //decimalSeparator: ','
                    }], [{
                        columnWidth: 1,
                        fieldLabel: this.app.i18n._('Manufacturer'),
                        name: 'manufacturer'
                    }], [{
                        columnWidth: 1,
                        fieldLabel: this.app.i18n._('Category'),
                        name: 'category'
                    }], [{
                        columnWidth: 1,
                        fieldLabel: this.app.i18n._('Description'),
                        emptyText: this.app.i18n._('Enter description...'),
                        name: 'description',
                        xtype: 'textarea',
                        height: 150
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
                            app: 'Sales',
                            showAddNoteForm: false,
                            border: false,
                            bodyStyle: 'border:1px solid #B5B8C8;'
                        }),
                        new Tine.widgets.tags.TagPanel({
                            app: 'Sales',
                            border: false,
                            bodyStyle: 'border:1px solid #B5B8C8;'
                        })
                    ]
                }]
            }, new Tine.widgets.activities.ActivitiesTabPanel({
                app: this.appName,
                record_id: this.record.id,
                record_model: this.appName + '_Model_' + this.recordClass.getMeta('modelName')
            })
            ]
        };
    }
});

/**
 * Sales Edit Popup
 */
Tine.Sales.ProductEditDialog.openWindow = function (config) {
    var id = (config.record && config.record.id) ? config.record.id : 0;
    var window = Tine.WindowFactory.getWindow({
        width: 600,
        height: 500,
        name: Tine.Sales.ProductEditDialog.prototype.windowNamePrefix + id,
        contentPanelConstructor: 'Tine.Sales.ProductEditDialog',
        contentPanelConstructorConfig: config
    });
    return window;
};
