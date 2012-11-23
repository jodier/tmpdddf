/*!
 * Tine 2.0 - Addressbook 
 * Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 * http://www.gnu.org/licenses/agpl.html AGPL Version 3
 */
/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2010 Metaways Infosystems GmbH (http://www.metaways.de)
 * 
 * TODO         add 'one of / in' operator?
 */
Ext.ns('Tine.Addressbook');

/**
 * @namespace   Tine.Addressbook
 * @class       Tine.Addressbook.ListMemberFilterModel
 * @extends     Tine.widgets.grid.FilterModel
 * 
 * @author      Philipp Schüle <p.schuele@metaways.de>
 */
Tine.Addressbook.ListMemberFilterModel = Ext.extend(Tine.widgets.grid.FilterModel, {
    /**
     * @property Tine.Tinebase.Application app
     */
    app: null,
    
    field: 'list',
    defaultOperator: 'equals',
    
    /**
     * @private
     */
    initComponent: function() {
        Tine.Addressbook.ListMemberFilterModel.superclass.initComponent.call(this);
        
        this.app = Tine.Tinebase.appMgr.get('Addressbook');
        
        this.operators = ['equals'];
        this.label = this.app.i18n._('Member of List');
    },
    
    /**
     * value renderer
     * 
     * @param {Ext.data.Record} filter line
     * @param {Ext.Element} element to render to 
     */
    valueRenderer: function(filter, el) {
        //var value = new Tine.Addressbook.ListMemberFilterModelValueField({
        var value = new Tine.Tinebase.widgets.form.RecordPickerComboBox({
            blurOnSelect: true,
            recordClass: Tine.Addressbook.Model.List,
            //app: this.app,
            filter: filter,
            width: 200,
            id: 'tw-ftb-frow-valuefield-' + filter.id,
            value: filter.data.value ? filter.data.value : this.defaultValue,
            renderTo: el
        });
        value.on('select', this.onFiltertrigger, this);
        return value;
    }
});

Tine.widgets.grid.FilterToolbar.FILTERS['addressbook.listMember'] = Tine.Addressbook.ListMemberFilterModel;

//Tine.Addressbook.ListMemberFilterModelValueField = Ext.extend(Ext.ux.form.LayerCombo, {
//});
/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 */
 
/*global Ext, Tine*/
 
Ext.ns('Tine.Addressbook.Model');

// TODO: move this into model definition and replace uscases (?) with getter fn
Tine.Addressbook.Model.ContactArray = Tine.Tinebase.Model.genericFields.concat([
    {name: 'id', omitDuplicateResolving: true},
    {name: 'tid', omitDuplicateResolving: true},
    {name: 'private', omitDuplicateResolving: true},
    {name: 'cat_id', omitDuplicateResolving: true},
    {name: 'n_family', label: 'Last Name', group: 'Name' },//_('Last Name') _('Name')
    {name: 'n_given', label: 'First Name', group: 'Name' }, //_('First Name')
    {name: 'n_middle', label: 'Middle Name', group: 'Name' }, //_('Middle Name')
    {name: 'n_prefix', label: 'Title', group: 'Name' }, //_('Title')
    {name: 'n_suffix', label: 'Suffix', group: 'Name'}, //_('Suffix')
    {name: 'n_fn', label: 'Display Name', group: 'Name', omitDuplicateResolving: true }, //_('Display Name')
    {name: 'n_fileas', group: 'Name', omitDuplicateResolving: true },
    {name: 'bday', label: 'Birthday', type: 'date', dateFormat: Date.patterns.ISO8601Long }, //_('Birthday')
    {name: 'org_name', label: 'Company', group: 'Company' }, //_('Company')
    {name: 'org_unit', label: 'Unit', group: 'Company' }, //_('Unit')
    {name: 'salutation', label: 'Salutation', group: 'Name' }, //_('Salutation')
    {name: 'title', label: 'Job Title', group: 'Company' }, //_('Job Title')
    {name: 'role', label: 'Job Role', group: 'Company' }, //_('Job Role')
    {name: 'assistent', group: 'Company', omitDuplicateResolving: true},
    {name: 'room', label: 'Room', group: 'Company' }, //_('Room')
    {name: 'adr_one_street', label: 'Street (Company Address)', group: 'Company Address' }, //_('Street (Company Address)')  _('Company Address')
    {name: 'adr_one_street2', label: 'Street 2 (Company Address)', group: 'Company Address' }, //_('Street 2 (Company Address)')
    {name: 'adr_one_locality', label: 'City (Company Address)', group: 'Company Address' }, //_('City (Company Address)')
    {name: 'adr_one_region', label: 'Region (Company Address)', group: 'Company Address' }, //_('Region (Company Address)')
    {name: 'adr_one_postalcode', label: 'Postal Code (Company Address)', group: 'Company Address' }, //_('Postal Code (Company Address)')
    {name: 'adr_one_countryname', label: 'Country (Company Address)', group: 'Company Address' }, //_('Country (Company Address)')
    {name: 'adr_one_lon', group: 'Company Address', omitDuplicateResolving: true },
    {name: 'adr_one_lat', group: 'Company Address', omitDuplicateResolving: true },
    {name: 'label', omitDuplicateResolving: true},
    {name: 'adr_two_street', label: 'Street (Private Address)', group: 'Private Address' }, //_('Street (Private Address)')  _('Private Address')
    {name: 'adr_two_street2', label: 'Street 2 (Private Address)', group: 'Private Address' }, //_('Street 2 (Private Address)')
    {name: 'adr_two_locality', label: 'City (Private Address)', group: 'Private Address' }, //_('City (Private Address)')
    {name: 'adr_two_region', label: 'Region (Private Address)', group: 'Private Address' }, //_('Region (Private Address)')
    {name: 'adr_two_postalcode', label: 'Postal Code (Private Address)', group: 'Private Address' }, //_('Postal Code (Private Address)')
    {name: 'adr_two_countryname', label: 'Country (Private Address)', group: 'Private Address' }, //_('Country (Private Address)')
    {name: 'adr_two_lon', group: 'Private Address', omitDuplicateResolving: true},
    {name: 'adr_two_lat', group: 'Private Address', omitDuplicateResolving: true},
    {name: 'tel_work', label: 'Phone', group: 'Company Communication' }, //_('Phone') _('Company Communication') 
    {name: 'tel_cell', label: 'Mobile', group: 'Company Communication' }, //_('Mobile')
    {name: 'tel_fax', label: 'Fax', group: 'Company Communication' }, //_('Fax')
    {name: 'tel_assistent', group: 'contact_infos', omitDuplicateResolving: true },
    {name: 'tel_car', group: 'contact_infos', omitDuplicateResolving: true },
    {name: 'tel_pager', group: 'contact_infos', omitDuplicateResolving: true },
    {name: 'tel_home', label: 'Phone (private)', group: 'Private Communication' }, //_('Phone (private)') _('Private Communication') 
    {name: 'tel_fax_home', label: 'Fax (private)', group: 'Private Communication' }, //_('Fax (private)')
    {name: 'tel_cell_private', label: 'Mobile (private)', group: 'Private Communication' }, //_('Mobile (private)')
    {name: 'tel_other', group: 'contact_infos', omitDuplicateResolving: true },
    {name: 'tel_prefer', group: 'contact_infos', omitDuplicateResolving: true},
    {name: 'email', label: 'E-Mail', group: 'Company Communication' }, //_('E-Mail')
    {name: 'email_home', label: 'E-Mail (private)', group: 'Private Communication' }, //_('E-Mail (private)')
    {name: 'url', label: 'Web', group: 'Company Communication' }, //_('Web')
    {name: 'url_home', label: 'Web (private)', group: 'Private Communication' }, //_('Web (private)')
    {name: 'freebusy_uri', omitDuplicateResolving: true},
    {name: 'calendar_uri', omitDuplicateResolving: true},
    {name: 'note', label: 'Description' }, //_('Description')
    {name: 'tz', omitDuplicateResolving: true},
    {name: 'pubkey', omitDuplicateResolving: true},
    {name: 'jpegphoto', omitDuplicateResolving: true},
    {name: 'account_id', isMetaField: true},
    {name: 'tags'},
    {name: 'notes', omitDuplicateResolving: true},
    {name: 'relations', omitDuplicateResolving: true},
    {name: 'customfields', isMetaField: true},
    {name: 'type', omitDuplicateResolving: true}
]);

/**
 * @namespace   Tine.Addressbook.Model
 * @class       Tine.Addressbook.Model.Contact
 * @extends     Tine.Tinebase.data.Record
 * @constructor
 * Model of a contact<br>
 * 
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 */
Tine.Addressbook.Model.Contact = Tine.Tinebase.data.Record.create(Tine.Addressbook.Model.ContactArray, {
    appName: 'Addressbook',
    modelName: 'Contact',
    idProperty: 'id',
    titleProperty: 'n_fn',
    // ngettext('Contact', 'Contacts', n); gettext('Contacts');
    recordName: 'Contact',
    recordsName: 'Contacts',
    containerProperty: 'container_id',
    // ngettext('Addressbook', 'Addressbooks', n); gettext('Addressbooks');
    containerName: 'Addressbook',
    containersName: 'Addressbooks',
    copyOmitFields: ['account_id', 'type'],
    
    /**
     * returns true if record has an email address
     * @return {Boolean}
     */
    hasEmail: function() {
        return this.get('email') || this.get('email_home');
    },
    
    /**
     * returns true prefered email if available
     * @return {String}
     */
    getPreferedEmail: function(prefered) {
        var prefered = prefered || 'email',
            other = prefered == 'email' ? 'email_home' : 'email';
            
        return (this.get(prefered) || this.get(other));
    }
});

/**
 * get filtermodel of contact model
 * 
 * @namespace Tine.Addressbook.Model
 * @static
 * @return {Array} filterModel definition
 */ 
Tine.Addressbook.Model.Contact.getFilterModel = function() {
    var app = Tine.Tinebase.appMgr.get('Addressbook');
    
    var typeStore = [['contact', app.i18n._('Contact')], ['user', app.i18n._('User Account')]];
    
    return [
        {label: _('Quick search'),                                                      field: 'query',              operators: ['contains']},
        {filtertype: 'tine.widget.container.filtermodel', app: app, recordClass: Tine.Addressbook.Model.Contact},
        {filtertype: 'addressbook.listMember', app: app},
        {label: app.i18n._('First Name'),                                               field: 'n_given' },
        {label: app.i18n._('Last Name'),                                                field: 'n_family'},
        {label: app.i18n._('Company'),                                                  field: 'org_name'},
        {label: app.i18n._('Unit'),                                                     field: 'org_unit'},
        {label: app.i18n._('Phone'),                                                    field: 'telephone',          operators: ['contains']},
        {label: app.i18n._('Job Title'),                                                field: 'title'},
        {label: app.i18n._('Job Role'),                                                 field: 'role'},
        {label: app.i18n._('Description'),                                              field: 'note'},
        {label: app.i18n._('E-Mail'),                                                   field: 'email_query',        operators: ['contains']},
        {filtertype: 'tinebase.tag', app: app},
        //{label: app.i18n._('Birthday'),    field: 'bday', valueType: 'date'},
        {label: app.i18n._('Street') + ' (' + app.i18n._('Company Address') + ')',      field: 'adr_one_street',     defaultOperator: 'equals'},
        {label: app.i18n._('Region') + ' (' + app.i18n._('Company Address') + ')',      field: 'adr_one_region',     defaultOperator: 'equals'},
        {label: app.i18n._('Postal Code') + ' (' + app.i18n._('Company Address') + ')', field: 'adr_one_postalcode', defaultOperator: 'equals'},
        {label: app.i18n._('City') + '  (' + app.i18n._('Company Address') + ')',       field: 'adr_one_locality'},
        {label: app.i18n._('Country') + '  (' + app.i18n._('Company Address') + ')',    field: 'adr_one_countryname', valueType: 'country'},
        {label: app.i18n._('Street') + ' (' + app.i18n._('Private Address') + ')',      field: 'adr_two_street',     defaultOperator: 'equals'},
        {label: app.i18n._('Region') + ' (' + app.i18n._('Private Address') + ')',      field: 'adr_two_region',     defaultOperator: 'equals'},
        {label: app.i18n._('Postal Code') + ' (' + app.i18n._('Private Address') + ')', field: 'adr_two_postalcode', defaultOperator: 'equals'},
        {label: app.i18n._('City') + ' (' + app.i18n._('Private Address') + ')',        field: 'adr_two_locality'},
        {label: app.i18n._('Country') + '  (' + app.i18n._('Private Address') + ')',    field: 'adr_two_countryname', valueType: 'country'},
        {label: app.i18n._('Type'), defaultValue: 'contact', valueType: 'combo',        field: 'type',               store: typeStore},
        {label: _('Last Modified Time'),                                                field: 'last_modified_time', valueType: 'date'},
        {label: _('Last Modified By'),                                                  field: 'last_modified_by',   valueType: 'user'},
        {label: _('Creation Time'),                                                     field: 'creation_time',      valueType: 'date'},
        {label: _('Created By'),                                                        field: 'created_by',         valueType: 'user'}
    ];
};
    
/**
 * default timesheets backend
 */
Tine.Addressbook.contactBackend = new Tine.Tinebase.data.RecordProxy({
    appName: 'Addressbook',
    modelName: 'Contact',
    recordClass: Tine.Addressbook.Model.Contact
});

/**
 * list model
 */
Tine.Addressbook.Model.List = Tine.Tinebase.data.Record.create([
   {name: 'id'},
   {name: 'container_id'},
   {name: 'created_by'},
   {name: 'creation_time'},
   {name: 'last_modified_by'},
   {name: 'last_modified_time'},
   {name: 'is_deleted'},
   {name: 'deleted_time'},
   {name: 'deleted_by'},
   {name: 'name'},
   {name: 'description'},
   {name: 'members'},
   {name: 'email'},
   {name: 'type'},
   {name: 'group_id'}
], {
    appName: 'Addressbook',
    modelName: 'List',
    idProperty: 'id',
    titleProperty: 'name',
    // ngettext('List', 'Lists', n); gettext('Lists');
    recordName: 'List',
    recordsName: 'Lists',
    containerProperty: 'container_id',
    // ngettext('Addressbook', 'Addressbooks', n); gettext('Addressbooks');
    containerName: 'Addressbook',
    containersName: 'Addressbooks',
    copyOmitFields: ['group_id']
});
/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2007-2008 Metaways Infosystems GmbH (http://www.metaways.de)
 */

Ext.ns('Tine.Addressbook');

/**
 * @namespace   Tine.Addressbook
 * @class       Tine.Addressbook.Application
 * @extends     Tine.Tinebase.Application
 * Addressbook Application Object <br>
 * 
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 */
Tine.Addressbook.Application = Ext.extend(Tine.Tinebase.Application, {
    
    /**
     * auto hook text _('New Contact')
     */
    addButtonText: 'New Contact',
    
    /**
     * Get translated application title of the calendar application
     * 
     * @return {String}
     */
    getTitle: function() {
        return this.i18n.ngettext('Addressbook', 'Addressbooks', 1);
    }
});

/**
 * @namespace   Tine.Addressbook
 * @class       Tine.Addressbook.MainScreen
 * @extends     Tine.widgets.MainScreen
 * MainScreen of the Addressbook Application <br>
 * 
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 */
Tine.Addressbook.MainScreen = Ext.extend(Tine.widgets.MainScreen, {
    activeContentType: 'Contact'
});


Tine.Addressbook.ContactTreePanel = function(config) {
    Ext.apply(this, config);
    
    this.id = 'Addressbook_Tree';
    this.filterMode = 'filterToolbar';
    this.recordClass = Tine.Addressbook.Model.Contact;
    Tine.Addressbook.ContactTreePanel.superclass.constructor.call(this);
};
Ext.extend(Tine.Addressbook.ContactTreePanel , Tine.widgets.container.TreePanel);

Tine.Addressbook.handleRequestException = Tine.Tinebase.ExceptionHandler.handleRequestException;

Tine.Addressbook.ContactFilterPanel = function(config) {
    Ext.apply(this, config);
    Tine.Addressbook.ContactFilterPanel.superclass.constructor.call(this);
};

Ext.extend(Tine.Addressbook.ContactFilterPanel, Tine.widgets.persistentfilter.PickerPanel, {
    filter: [{field: 'model', operator: 'equals', value: 'Addressbook_Model_ContactFilter'}]
});/**
 * Tine 2.0
 * 
 * @package     Addressbook
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schuele <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2007-2009 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 * TODO         add preference for sending mails with felamimail or mailto?
 */
 
Ext.ns('Tine.Addressbook');

/**
 * the details panel (shows contact details)
 * 
 * @namespace   Tine.Addressbook
 * @class       Tine.Addressbook.ContactGridDetailsPanel
 * @extends     Tine.widgets.grid.DetailsPanel
 */
Tine.Addressbook.ContactGridDetailsPanel = Ext.extend(Tine.widgets.grid.DetailsPanel, {
    
    il8n: null,
    felamimail: false,
    
    /**
     * init
     */
    initComponent: function() {

        // init templates
        this.initTemplate();
        this.initDefaultTemplate();
        
        Tine.Addressbook.ContactGridDetailsPanel.superclass.initComponent.call(this);
    },

    /**
     * add on click event after render
     */
    afterRender: function() {
        Tine.Addressbook.ContactGridDetailsPanel.superclass.afterRender.apply(this, arguments);
        
        if (this.felamimail === true) {
            this.body.on('click', this.onClick, this);
        }
    },
    
    /**
     * init default template
     */
    initDefaultTemplate: function() {
        
        this.defaultTpl = new Ext.XTemplate(
            '<div class="preview-panel-timesheet-nobreak">',    
                '<!-- Preview contacts -->',
                '<div class="preview-panel preview-panel-timesheet-left">',
                    '<div class="bordercorner_1"></div>',
                    '<div class="bordercorner_2"></div>',
                    '<div class="bordercorner_3"></div>',
                    '<div class="bordercorner_4"></div>',
                    '<div class="preview-panel-declaration">' + this.il8n._('Contacts') + '</div>',
                    '<div class="preview-panel-timesheet-leftside preview-panel-left">',
                        '<span class="preview-panel-bold">',
                            this.il8n._('Select contact') + '<br/>',
                            '<br/>',
                            '<br/>',
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
                '<!-- Preview xxx -->',
                '<div class="preview-panel-timesheet-right">',
                    '<div class="bordercorner_gray_1"></div>',
                    '<div class="bordercorner_gray_2"></div>',
                    '<div class="bordercorner_gray_3"></div>',
                    '<div class="bordercorner_gray_4"></div>',
                    '<div class="preview-panel-declaration"></div>',
                    '<div class="preview-panel-timesheet-leftside preview-panel-left">',
                        '<span class="preview-panel-bold">',
                            '<br/>',
                            '<br/>',
                            '<br/>',
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
            '</div>'        
        );
    },
    
    /**
     * init single contact template (this.tpl)
     */
    initTemplate: function() {
        this.tpl = new Ext.XTemplate(
            '<tpl for=".">',
                '<div class="preview-panel-adressbook-nobreak">',
                '<div class="preview-panel-left">',                
                    '<!-- Preview image -->',
                    '<div class="preview-panel preview-panel-left preview-panel-image">',
                        '<div class="bordercorner_1"></div>',
                        '<div class="bordercorner_2"></div>',
                        '<div class="bordercorner_3"></div>',
                        '<div class="bordercorner_4"></div>',
                        '<img src="{[this.getImageUrl(values.jpegphoto, 90, 113, values)]}"/>',
                    '</div>',
                
                    '<!-- Preview office -->',
                    '<div class="preview-panel preview-panel-office preview-panel-left">',                
                        '<div class="bordercorner_1"></div>',
                        '<div class="bordercorner_2"></div>',
                        '<div class="bordercorner_3"></div>',
                        '<div class="bordercorner_4"></div>',
                        '<div class="preview-panel-declaration">' + this.il8n._('Company') + '</div>',
                        '<div class="preview-panel-address preview-panel-left">',
                            '<span class="preview-panel-bold">{[this.encode(values.org_name, "mediumtext")]}{[this.encode(values.org_unit, "prefix", " / ")]}</span><br/>',
                            '{[this.encode(values.adr_one_street)]}<br/>',
                            '{[this.encode(values.adr_one_postalcode, " ")]}{[this.encode(values.adr_one_locality)]}<br/>',
                            '{[this.encode(values.adr_one_region, " / ")]}{[this.encode(values.adr_one_countryname, "country")]}<br/>',
                        '</div>',
                        '<div class="preview-panel-contact preview-panel-right">',
                            '<span class="preview-panel-symbolcompare">' + this.il8n._('Phone') + '</span>{[this.encode(values.tel_work)]}<br/>',
                            '<span class="preview-panel-symbolcompare">' + this.il8n._('Mobile') + '</span>{[this.encode(values.tel_cell)]}<br/>',
                            '<span class="preview-panel-symbolcompare">' + this.il8n._('Fax') + '</span>{[this.encode(values.tel_fax)]}<br/>',
                            '<span class="preview-panel-symbolcompare">' + this.il8n._('E-Mail') 
                                + '</span>{[this.getMailLink(values.email, ' + this.felamimail + ')]}<br/>',
                            '<span class="preview-panel-symbolcompare">' + this.il8n._('Web') + '</span><a href="{[this.encode(values.url, "href")]}" target="_blank">{[this.encode(values.url, "shorttext")]}</a><br/>',
                        '</div>',
                    '</div>',
                '</div>',

                '<!-- Preview privat -->',
                '<div class="preview-panel preview-panel-privat preview-panel-left">',                
                    '<div class="bordercorner_1"></div>',
                    '<div class="bordercorner_2"></div>',
                    '<div class="bordercorner_3"></div>',
                    '<div class="bordercorner_4"></div>',
                    '<div class="preview-panel-declaration">' + this.il8n._('Private') + '</div>',
                    '<div class="preview-panel-address preview-panel-left">',
                        '<span class="preview-panel-bold">{[this.encode(values.n_fn)]}</span><br/>',
                        '{[this.encode(values.adr_two_street)]}<br/>',
                        '{[this.encode(values.adr_two_postalcode, " ")]}{[this.encode(values.adr_two_locality)]}<br/>',
                        '{[this.encode(values.adr_two_region, " / ")]}{[this.encode(values.adr_two_countryname, "country")]}<br/>',
                    '</div>',
                    '<div class="preview-panel-contact preview-panel-right">',
                        '<span class="preview-panel-symbolcompare">' + this.il8n._('Phone') + '</span>{[this.encode(values.tel_home)]}<br/>',
                        '<span class="preview-panel-symbolcompare">' + this.il8n._('Mobile') + '</span>{[this.encode(values.tel_cell_private)]}<br/>',
                        '<span class="preview-panel-symbolcompare">' + this.il8n._('Fax') + '</span>{[this.encode(values.tel_fax_home)]}<br/>',
                        '<span class="preview-panel-symbolcompare">' + this.il8n._('E-Mail') 
                            + '</span>{[this.getMailLink(values.email_home, ' + this.felamimail + ')]}<br/>',
                        '<span class="preview-panel-symbolcompare">' + this.il8n._('Web') + '</span><a href="{[this.encode(values.url, "href")]}" target="_blank">{[this.encode(values.url_home, "shorttext")]}</a><br/>',
                    '</div>',                
                '</div>',
                
                '<!-- Preview info -->',
                '<div class="preview-panel-description preview-panel-left" ext:qtip="{[this.encode(values.note)]}">',
                    '<div class="bordercorner_gray_1"></div>',
                    '<div class="bordercorner_gray_2"></div>',
                    '<div class="bordercorner_gray_3"></div>',
                    '<div class="bordercorner_gray_4"></div>',
                    '<div class="preview-panel-declaration">' + this.il8n._('Info') + '</div>',
                    '{[this.encode(values.note, "longtext")]}',
                '</div>',
                '</div>',
                //  '{[this.getTags(values.tags)]}',
            '</tpl>',
            {
                /**
                 * encode
                 */
                encode: function(value, type, prefix) {
                    //var metrics = Ext.util.TextMetrics.createInstance('previewPanel');
                    if (value) {
                        if (type) {
                            switch (type) {
                                case 'country':
                                    value = Locale.getTranslationData('CountryList', value);
                                    break;
                                case 'longtext':
                                    value = Ext.util.Format.ellipsis(value, 135);
                                    break;
                                case 'mediumtext':
                                    value = Ext.util.Format.ellipsis(value, 30);
                                    break;
                                case 'shorttext':
                                    //console.log(metrics.getWidth(value));
                                    value = Ext.util.Format.ellipsis(value, 18);
                                    break;
                                case 'prefix':
                                    if (prefix) {
                                        value = prefix + value;
                                    }
                                    break;
                                case 'href':
                                    if (! String(value).match(/^(https?|ftps?)/)) {
                                        var adb = Tine.Tinebase.appMgr.get('Addressbook');
                                        return "javascript:Ext.Msg.alert('" + adb.i18n._('Insecure link') + "', '" + adb.i18n._('Please review this link in edit dialog.') + "');";
                                    }
                                    break;
                                default:
                                    value += type;
                            }
                        }
                        value = Ext.util.Format.htmlEncode(value);
                        return Ext.util.Format.nl2br(value);
                    } else {
                        return '';
                    }
                },
                
                /**
                 * get tags
                 * 
                 * TODO make it work
                 */
                getTags: function(value) {
                    var result = '';
                    for (var i=0; i<value.length; i++) {
                        result += value[i].name + ' ';
                    }
                    return result;
                },
                
                /**
                 * get image url
                 */
                getImageUrl: function(url, width, height, contact) {
                    var mtime = contact.last_modified_time || contact.creation_time;
                    if (url.match(/&/)) {
                        url = Ext.ux.util.ImageURL.prototype.parseURL(url);
                        url.width = width;
                        url.height = height;
                        url.ratiomode = 0;
                        url.mtime = Ext.isDate(mtime) ? mtime.getTime() : new Date().getTime();
                    }
                    return url;
                },

                /**
                 * get email link
                 */
                getMailLink: function(email, felamimail) {
                    if (! email) {
                        return '';
                    }
                    
                    email = this.encode(email);
                    var link = (felamimail === true) ? '#' : 'mailto:' + email;
                    var id = Ext.id() + ':' + email;
                    
                    return '<a href="' + link + '" class="tinebase-email-link" id="' + id + '">'
                        + Ext.util.Format.ellipsis(email, 18) + '</a>';
                }
            }
        );
    },
    
    /**
     * on click for compose mail
     * 
     * @param {} e
     * 
     * TODO check if account is configured?
     * TODO generalize that
     */
    onClick: function(e) {
        var target = e.getTarget('a[class=tinebase-email-link]');
        if (target) {
            var email = target.id.split(':')[1];
            var defaults = Tine.Felamimail.Model.Message.getDefaultData();
            defaults.to = [email];
            defaults.body = Tine.Felamimail.getSignature();
            
            var record = new Tine.Felamimail.Model.Message(defaults, 0);
            var popupWindow = Tine.Felamimail.MessageEditDialog.openWindow({
                record: record
            });
        }
    }
});
/*
 * Tine 2.0
 * 
 * @package     Addressbook
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.ns('Tine.Addressbook');

/**
 * Contact grid panel
 * 
 * @namespace   Tine.Addressbook
 * @class       Tine.Addressbook.ContactGridPanel
 * @extends     Tine.widgets.grid.GridPanel
 * 
 * <p>Contact Grid Panel</p>
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Addressbook.ContactGridPanel
 */
Tine.Addressbook.ContactGridPanel = Ext.extend(Tine.widgets.grid.GridPanel, {
    /**
     * record class
     * @cfg {Tine.Addressbook.Model.Contact} recordClass
     */
    recordClass: Tine.Addressbook.Model.Contact,
    
    /**
     * grid specific
     * @private
     */ 
    defaultSortInfo: {field: 'n_fileas', direction: 'ASC'},
    gridConfig: {
        autoExpandColumn: 'n_fileas',
        enableDragDrop: true,
        ddGroup: 'containerDDGroup'
    },
    copyEditAction: true,
    felamimail: false,
    multipleEdit: true,
    duplicateResolvable: true,
    
    /**
     * @cfg {Bool} hasDetailsPanel 
     */
    hasDetailsPanel: true,
    
    /**
     * inits this cmp
     * @private
     */
    initComponent: function() {
        this.recordProxy = Tine.Addressbook.contactBackend;
        
        // check if felamimail is installed and user has run right and wants to use felamimail in adb
        if (Tine.Felamimail && Tine.Tinebase.common.hasRight('run', 'Felamimail') && Tine.Felamimail.registry.get('preferences').get('useInAdb')) {
            this.felamimail = (Tine.Felamimail.registry.get('preferences').get('useInAdb') == 1);
        }
        this.gridConfig.cm = this.getColumnModel();
        this.filterToolbar = this.filterToolbar || this.getFilterToolbar();

        if (this.hasDetailsPanel) {
            this.detailsPanel = this.getDetailsPanel();
        }
        
        this.plugins = this.plugins || [];
        this.plugins.push(this.filterToolbar);
        
        Tine.Addressbook.ContactGridPanel.superclass.initComponent.call(this);
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
     * returns array with columns
     * 
     * @return {Array}
     */
    getColumns: function() {
        return [
            { id: 'tid', header: this.app.i18n._('Type'), dataIndex: 'tid', width: 30, renderer: this.contactTidRenderer.createDelegate(this), hidden: false },
            { id: 'tags', header: this.app.i18n._('Tags'), dataIndex: 'tags', width: 50, renderer: Tine.Tinebase.common.tagsRenderer, sortable: false, hidden: false  },
            { id: 'salutation', header: this.app.i18n._('Salutation'), dataIndex: 'salutation', renderer: Tine.Tinebase.widgets.keyfield.Renderer.get('Addressbook', 'contactSalutation') },
            { id: 'n_family', header: this.app.i18n._('Last Name'), dataIndex: 'n_family' },
            { id: 'n_given', header: this.app.i18n._('First Name'), dataIndex: 'n_given', width: 80 },
            { id: 'n_fn', header: this.app.i18n._('Full Name'), dataIndex: 'n_fn' },
            { id: 'n_fileas', header: this.app.i18n._('Display Name'), dataIndex: 'n_fileas', hidden: false},
            { id: 'org_name', header: this.app.i18n._('Company'), dataIndex: 'org_name', width: 120, hidden: false },
            { id: 'org_unit', header: this.app.i18n._('Unit'), dataIndex: 'org_unit'  },
            { id: 'title', header: this.app.i18n._('Job Title'), dataIndex: 'title' },
            { id: 'role', header: this.app.i18n._('Job Role'), dataIndex: 'role' },
            { id: 'room', header: this.app.i18n._('Room'), dataIndex: 'room' },
            { id: 'adr_one_street', header: this.app.i18n._('Street'), dataIndex: 'adr_one_street' },
            { id: 'adr_one_locality', header: this.app.i18n._('City'), dataIndex: 'adr_one_locality', width: 150, hidden: false },
            { id: 'adr_one_region', header: this.app.i18n._('Region'), dataIndex: 'adr_one_region' },
            { id: 'adr_one_postalcode', header: this.app.i18n._('Postalcode'), dataIndex: 'adr_one_postalcode' },
            { id: 'adr_one_countryname', header: this.app.i18n._('Country'), dataIndex: 'adr_one_countryname' },
            { id: 'adr_two_street', header: this.app.i18n._('Street (private)'), dataIndex: 'adr_two_street' },
            { id: 'adr_two_locality', header: this.app.i18n._('City (private)'), dataIndex: 'adr_two_locality' },
            { id: 'adr_two_region', header: this.app.i18n._('Region (private)'), dataIndex: 'adr_two_region' },
            { id: 'adr_two_postalcode', header: this.app.i18n._('Postalcode (private)'), dataIndex: 'adr_two_postalcode' },
            { id: 'adr_two_countryname', header: this.app.i18n._('Country (private)'), dataIndex: 'adr_two_countryname' },
            { id: 'email', header: this.app.i18n._('Email'), dataIndex: 'email', width: 150, hidden: false },
            { id: 'tel_work', header: this.app.i18n._('Phone'), dataIndex: 'tel_work', hidden: false },
            { id: 'tel_cell', header: this.app.i18n._('Mobile'), dataIndex: 'tel_cell', hidden: false },
            { id: 'tel_fax', header: this.app.i18n._('Fax'), dataIndex: 'tel_fax' },
            { id: 'tel_car', header: this.app.i18n._('Car phone'), dataIndex: 'tel_car' },
            { id: 'tel_pager', header: this.app.i18n._('Pager'), dataIndex: 'tel_pager' },
            { id: 'tel_home', header: this.app.i18n._('Phone (private)'), dataIndex: 'tel_home' },
            { id: 'tel_fax_home', header: this.app.i18n._('Fax (private)'), dataIndex: 'tel_fax_home' },
            { id: 'tel_cell_private', header: this.app.i18n._('Mobile (private)'), dataIndex: 'tel_cell_private' },
            { id: 'email_home', header: this.app.i18n._('Email (private)'), dataIndex: 'email_home' },
            { id: 'url', header: this.app.i18n._('Web'), dataIndex: 'url' },
            { id: 'url_home', header: this.app.i18n._('URL (private)'), dataIndex: 'url_home' },
            { id: 'note', header: this.app.i18n._('Note'), dataIndex: 'note' },
            { id: 'tz', header: this.app.i18n._('Timezone'), dataIndex: 'tz' },
            { id: 'geo', header: this.app.i18n._('Geo'), dataIndex: 'geo' },
            { id: 'bday', header: this.app.i18n._('Birthday'), dataIndex: 'bday', renderer: Tine.Tinebase.common.dateRenderer }
        ].concat(this.getModlogColumns().concat(this.getCustomfieldColumns()));
    },
    
    /**
     * @private
     */
    initActions: function() {
        this.actions_exportContact = new Ext.Action({
            requiredGrant: 'exportGrant',
            text: String.format(this.app.i18n.ngettext('Export {0}', 'Export {0}', 50), this.i18nRecordsName),
            singularText: String.format(this.app.i18n.ngettext('Export {0}', 'Export {0}', 1), this.i18nRecordName),
            pluralText:  String.format(this.app.i18n.ngettext('Export {0}', 'Export {0}', 1), this.i18nRecordsName),
            translationObject: this.app.i18n,
            iconCls: 'action_export',
            scope: this,
            disabled: true,
            allowMultiple: true,
            menu: {
                items: [
                    new Tine.widgets.grid.ExportButton({
                        text: this.app.i18n._('Export as PDF'),
                        iconCls: 'action_exportAsPdf',
                        format: 'pdf',
                        exportFunction: 'Addressbook.exportContacts',
                        gridPanel: this
                    }),
                    new Tine.widgets.grid.ExportButton({
                        text: this.app.i18n._('Export as CSV'),
                        iconCls: 'tinebase-action-export-csv',
                        format: 'csv',
                        exportFunction: 'Addressbook.exportContacts',
                        gridPanel: this
                    }),
                    new Tine.widgets.grid.ExportButton({
                        text: this.app.i18n._('Export as ODS'),
                        format: 'ods',
                        iconCls: 'tinebase-action-export-ods',
                        exportFunction: 'Addressbook.exportContacts',
                        gridPanel: this
                    }),
                    new Tine.widgets.grid.ExportButton({
                        text: this.app.i18n._('Export as XLS'),
                        format: 'xls',
                        iconCls: 'tinebase-action-export-xls',
                        exportFunction: 'Addressbook.exportContacts',
                        gridPanel: this
                    }),
                    new Tine.widgets.grid.ExportButton({
                        text: this.app.i18n._('Export as ...'),
                        iconCls: 'tinebase-action-export-xls',
                        exportFunction: 'Addressbook.exportContacts',
                        showExportDialog: true,
                        gridPanel: this
                    })
                ]
            }
        });

        this.actions_import = new Ext.Action({
            //requiredGrant: 'addGrant',
            text: this.app.i18n._('Import contacts'),
            disabled: false,
            handler: this.onImport,
            iconCls: 'action_import',
            scope: this,
            allowMultiple: true
        });
        
        // register actions in updater
        this.actionUpdater.addActions([
            this.actions_exportContact,
            this.actions_import
        ]);
        
        Tine.Addressbook.ContactGridPanel.superclass.initActions.call(this);
    },
    
    /**
     * add custom items to action toolbar
     * 
     * @return {Object}
     */
    getActionToolbarItems: function() {
        return [
            {
                xtype: 'buttongroup',
                columns: 1,
                frame: false,
                items: [
                    this.actions_exportContact,
                    this.actions_import
                ]
            }
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
            this.actions_exportContact,
            '-'
        ];
        
        return items;
    },
    
    /**
     * import contacts
     * 
     * @param {Button} btn 
     * 
     * TODO generalize this & the import button
     */
    onImport: function(btn) {
        var popupWindow = Tine.widgets.dialog.ImportDialog.openWindow({
            appName: 'Addressbook',
            modelName: 'Contact',
            defaultImportContainer: this.app.getMainScreen().getWestPanel().getContainerTreePanel().getDefaultContainer('defaultAddressbook'),
            
            // update grid after import
            listeners: {
                scope: this,
                'finish': function() {
                    this.loadGridData({
                        preserveCursor:     false, 
                        preserveSelection:  false, 
                        preserveScroller:   false,
                        removeStrategy:     'default'
                    });
                }
            }
        });
    },
        
    /**
     * tid renderer
     * 
     * @private
     * @return {String} HTML
     */
    contactTidRenderer: function(data, cell, record) {
        
        switch(record.get('type')) {
            case 'user':
                return '<img src="images/oxygen/16x16/actions/user-female.png" width="12" height="12" alt="contact" ext:qtip="' + Ext.util.Format.htmlEncode(this.app.i18n._("Internal Contact")) + '"/>';
            default:
                return "<img src='images/oxygen/16x16/actions/user.png' width='12' height='12' alt='contact'/>";
        }
    },
    
    /**
     * returns details panel
     * 
     * @private
     * @return {Tine.Addressbook.ContactGridDetailsPanel}
     */
    getDetailsPanel: function() {
        return new Tine.Addressbook.ContactGridDetailsPanel({
            gridpanel: this,
            il8n: this.app.i18n,
            felamimail: this.felamimail
        });
    }
});
/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2010 Metaways Infosystems GmbH (http://www.metaways.de)
 */
Ext.ns('Tine.Calendar');

/**
 * @namespace   Tine.Calendar
 * @class       Tine.Addressbook.ContactFilterModel
 * @extends     Tine.widgets.grid.FilterModel
 * 
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 */
Tine.Addressbook.ContactFilterModel = Ext.extend(Tine.widgets.grid.FilterModel, {
    /**
     * @property Tine.Tinebase.Application app
     */
    app: null,
    
    field: 'contact_id',
    defaultOperator: 'equals',
    
    /**
     * @private
     */
    initComponent: function() {
        Tine.Addressbook.ContactFilterModel.superclass.initComponent.call(this);
        
        this.app = Tine.Tinebase.appMgr.get('Addressbook');
        
        this.operators = ['equals'/*, 'notin'*/];
        this.label = this.label || this.app.i18n._('Contact');
    },
    
    /**
     * value renderer
     * 
     * @param {Ext.data.Record} filter line
     * @param {Ext.Element} element to render to 
     */
    valueRenderer: function(filter, el) {
        var value = new Tine.Addressbook.SearchCombo({
            app: this.app,
            filter: filter,
            width: 200,
            listWidth: 400,
            listAlign : 'tr-br?',
            id: 'tw-ftb-frow-valuefield-' + filter.id,
            value: filter.data.value ? filter.data.value : this.defaultValue,
            renderTo: el,
            getValue: function() {
                return this.selectedRecord.id;
            },
            onSelect: function(record) {
                this.setValue(record);
                this.collapse();
        
                this.fireEvent('select', this, record);
                if (this.blurOnSelect) {
                    this.fireEvent('blur', this);
                }
            },
            setValue: function(value) {
                this.selectedRecord = value;
                var displayValue = Tine.Calendar.AttendeeGridPanel.prototype.renderAttenderUserName.call(this, value);
                Tine.Addressbook.SearchCombo.superclass.setValue.call(this, displayValue);
            }
        });
        value.on('select', this.onFiltertrigger, this);
        return value;
    }
});

Tine.widgets.grid.FilterToolbar.FILTERS['addressbook.contact'] = Tine.Addressbook.ContactFilterModel;

/*
 * Tine 2.0
 * 
 * @package     Addressbook
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2009-2012 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */

/*global Ext, Tine*/

Ext.ns('Tine.Addressbook');

/**
 * @namespace   Tine.Addressbook
 * @class       Tine.Addressbook.ContactEditDialog
 * @extends     Tine.widgets.dialog.EditDialog
 * Addressbook Edit Dialog <br>
 * 
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 */
Tine.Addressbook.ContactEditDialog = Ext.extend(Tine.widgets.dialog.EditDialog, {
    
    /**
     * parse address button
     * @type Ext.Button 
     */
    parseAddressButton: null,
    
    windowNamePrefix: 'ContactEditWindow_',
    appName: 'Addressbook',
    recordClass: Tine.Addressbook.Model.Contact,
    showContainerSelector: true,
    multipleEdit: true,
    
    getFormItems: function () {
        if (Tine.Tinebase.registry.get('mapPanel') && Tine.widgets.MapPanel) {
            this.mapPanel = new Tine.Addressbook.MapPanel({
                layout: 'fit',
                title: this.app.i18n._('Map'),
                disabled: (Ext.isEmpty(this.record.get('adr_one_lon')) || Ext.isEmpty(this.record.get('adr_one_lat'))) && (Ext.isEmpty(this.record.get('adr_two_lon')) || Ext.isEmpty(this.record.get('adr_two_lat')))
            });
            
            Tine.widgets.dialog.MultipleEditDialogPlugin.prototype.registerSkipItem(this.mapPanel);
            
        } else {
            this.mapPanel = new Ext.Panel({
                layout: 'fit',
                title: this.app.i18n._('Map'),
                disabled: true,
                html: ''
            });
        }
        
        return {
            xtype: 'tabpanel',
            border: false,
            plain: true,
            activeTab: 0,
            plugins: [{
                ptype : 'ux.tabpanelkeyplugin'
            }],
            items: [{
                title: this.app.i18n.n_('Contact', 'Contacts', 1),
                border: false,
                frame: true,
                layout: 'border',
                items: [{
                    region: 'center',
                    layout: 'border',
                    items: [{
                        xtype: 'fieldset',
                        region: 'north',
                        autoHeight: true,
                        title: this.app.i18n._('Personal Information'),
                        items: [{
                            xtype: 'panel',
                            layout: 'fit',
                            width: 90,
                            height: 120,
                            style: {
                                position: 'absolute',
                                right: '10px',
                                top: Ext.isGecko ? '7px' : '19px',
                                'z-index': 100
                            },
                            items: [new Ext.ux.form.ImageField({
                                name: 'jpegphoto',
                                width: 90,
                                height: 120
                            })]
                        }, {
                            xtype: 'columnform',
                            items: [[
                                new Tine.Tinebase.widgets.keyfield.ComboBox({
                                fieldLabel: this.app.i18n._('Salutation'),
                                name: 'salutation',
                                app: 'Addressbook',
                                keyFieldName: 'contactSalutation',
                                value: '',
                                columnWidth: 0.35,
                                listeners: {
                                    scope: this,
                                    'select': function (combo, record, index) {
                                        var jpegphoto = this.getForm().findField('jpegphoto');
                                        // set new empty photo depending on chosen salutation only if user doesn't have own image
                                        if (Ext.isEmpty(jpegphoto.getValue()) && ! Ext.isEmpty(record.json.image)) {
                                            jpegphoto.setDefaultImage(record.json.image);
                                        }
                                    }
                                }
                            }), {
                                columnWidth: 0.65,
                                fieldLabel: this.app.i18n._('Title'), 
                                name: 'n_prefix',
                                maxLength: 64
                            }, {
                                width: 100,
                                hidden: true
                            }], [{
                                columnWidth: 0.35,
                                fieldLabel: this.app.i18n._('First Name'), 
                                name: 'n_given',
                                maxLength: 64
                            }, {
                                columnWidth: 0.30,
                                fieldLabel: this.app.i18n._('Middle Name'), 
                                name: 'n_middle',
                                maxLength: 64
                            }, {
                                columnWidth: 0.35,
                                fieldLabel: this.app.i18n._('Last Name'), 
                                name: 'n_family',
                                maxLength: 255
                            }, {
                                width: 100,
                                hidden: true
                            }], [{
                                columnWidth: 0.65,
                                xtype: 'mirrortextfield',
                                fieldLabel: this.app.i18n._('Company'), 
                                name: 'org_name',
                                maxLength: 255
                            }, {
                                columnWidth: 0.35,
                                fieldLabel: this.app.i18n._('Unit'), 
                                name: 'org_unit',
                                maxLength: 64
                            }, {
                                width: 100,
                                hidden: true
                            }], [{
                                columnWidth: 0.65,
                                xtype: 'combo',
                                fieldLabel: this.app.i18n._('Display Name'),
                                name: 'n_fn',
                                disabled: true
                            }, {
                                columnWidth: 0.35,
                                fieldLabel: this.app.i18n._('Job Title'),
                                name: 'title',
                                maxLength: 64
                            }, {
                                width: 100,
                                xtype: 'extuxclearabledatefield',
                                fieldLabel: this.app.i18n._('Birthday'),
                                name: 'bday'
                            }]/* move to seperate tab, [{
                                columnWidth: .4,
                                fieldLabel: this.app.i18n._('Suffix'), 
                                name:'n_suffix'
                            }, {
                                columnWidth: .4,
                                fieldLabel: this.app.i18n._('Job Role'), 
                                name:'role'
                            }, {
                                columnWidth: .2,
                                fieldLabel: this.app.i18n._('Room'), 
                                name:'room'
                            }]*/]
                        }]
                    }, {
                        xtype: 'fieldset',
                        region: 'center',
                        title: this.app.i18n._('Contact Information'),
                        autoScroll: true,
                        items: [{
                            xtype: 'columnform',
                            items: [[{
                                fieldLabel: this.app.i18n._('Phone'), 
                                labelIcon: 'images/oxygen/16x16/apps/kcall.png',
                                name: 'tel_work',
                                maxLength: 40
                            }, {
                                fieldLabel: this.app.i18n._('Mobile'),
                                labelIcon: 'images/oxygen/16x16/devices/phone.png',
                                name: 'tel_cell',
                                maxLength: 40
                            }, {
                                fieldLabel: this.app.i18n._('Fax'), 
                                labelIcon: 'images/oxygen/16x16/devices/printer.png',
                                name: 'tel_fax',
                                maxLength: 40
                            }], [{
                                fieldLabel: this.app.i18n._('Phone (private)'),
                                labelIcon: 'images/oxygen/16x16/apps/kcall.png',
                                name: 'tel_home',
                                maxLength: 40
                            }, {
                                fieldLabel: this.app.i18n._('Mobile (private)'),
                                labelIcon: 'images/oxygen/16x16/devices/phone.png',
                                name: 'tel_cell_private',
                                maxLength: 40
                            }, {
                                fieldLabel: this.app.i18n._('Fax (private)'), 
                                labelIcon: 'images/oxygen/16x16/devices/printer.png',
                                name: 'tel_fax_home',
                                maxLength: 40
                            }], [{
                                fieldLabel: this.app.i18n._('E-Mail'), 
                                labelIcon: 'images/oxygen/16x16/actions/kontact-mail.png',
                                name: 'email',
                                vtype: 'email',
                                maxLength: 64
                            }, {
                                fieldLabel: this.app.i18n._('E-Mail (private)'), 
                                labelIcon: 'images/oxygen/16x16/actions/kontact-mail.png',
                                name: 'email_home',
                                vtype: 'email',
                                maxLength: 64
                            }, {
                                xtype: 'mirrortextfield',
                                fieldLabel: this.app.i18n._('Web'),
                                labelIcon: 'images/oxygen/16x16/actions/network.png',
                                name: 'url',
                                maxLength: 128,
                                listeners: {
                                    scope: this,
                                    focus: function (field) {
                                        if (! field.getValue()) {
                                            field.setValue('http://www.');
                                            field.selectText.defer(100, field, [7, 11]);
                                        }
                                    },
                                    blur: function (field) {
                                        if (field.getValue() === 'http://www.') {
                                            field.setValue(null);
                                            field.validate();
                                        }
                                    }
                                }
                            }]]
                        }]
                    }, {
                        xtype: 'tabpanel',
                        region: 'south',
                        border: false,
                        deferredRender: false,
                        height: 124,
                        split: true,
                        activeTab: 0,
                        defaults: {
                            frame: true
                        },
                        items: [{
                            title: this.app.i18n._('Company Address'),
                            xtype: 'columnform',
                            items: [[{
                                fieldLabel: this.app.i18n._('Street'), 
                                name: 'adr_one_street',
                                maxLength: 64
                            }, {
                                fieldLabel: this.app.i18n._('Street 2'), 
                                name: 'adr_one_street2',
                                maxLength: 64
                            }, {
                                fieldLabel: this.app.i18n._('Region'),
                                name: 'adr_one_region',
                                maxLength: 64
                            }], [{
                                fieldLabel: this.app.i18n._('Postal Code'), 
                                name: 'adr_one_postalcode',
                                maxLength: 64
                            }, {
                                fieldLabel: this.app.i18n._('City'),
                                name: 'adr_one_locality',
                                maxLength: 64
                            }, {
                                xtype: 'widget-countrycombo',
                                fieldLabel: this.app.i18n._('Country'),
                                name: 'adr_one_countryname',
                                maxLength: 64
                            }]]
                        }, {
                            title: this.app.i18n._('Private Address'),
                            xtype: 'columnform',
                            items: [[{
                                fieldLabel: this.app.i18n._('Street'), 
                                name: 'adr_two_street',
                                maxLength: 64
                            }, {
                                fieldLabel: this.app.i18n._('Street 2'), 
                                name: 'adr_two_street2',
                                maxLength: 64
                            }, {
                                fieldLabel: this.app.i18n._('Region'),
                                name: 'adr_two_region',
                                maxLength: 64
                            }], [{
                                fieldLabel: this.app.i18n._('Postal Code'), 
                                name: 'adr_two_postalcode',
                                maxLength: 64
                            }, {
                                fieldLabel: this.app.i18n._('City'),
                                name: 'adr_two_locality',
                                maxLength: 64
                            }, {
                                xtype: 'widget-countrycombo',
                                fieldLabel: this.app.i18n._('Country'),
                                name: 'adr_two_countryname',
                                maxLength: 64
                            }]]
                        }]
                    }]
                }, {
                    // activities and tags
                    region: 'east',
                    layout: 'accordion',
                    animate: true,
                    width: 210,
                    split: true,
                    collapsible: true,
                    collapseMode: 'mini',
                    header: false,
                    margins: '0 5 0 5',
                    border: true,
                    items: [
                        new Ext.Panel({
                            // @todo generalise!
                            title: this.app.i18n._('Description'),
                            iconCls: 'descriptionIcon',
                            layout: 'form',
                            labelAlign: 'top',
                            border: false,
                            items: [{
                                style: 'margin-top: -4px; border 0px;',
                                labelSeparator: '',
                                xtype: 'textarea',
                                name: 'note',
                                hideLabel: true,
                                grow: false,
                                preventScrollbars: false,
                                anchor: '100% 100%',
                                emptyText: this.app.i18n._('Enter description'),
                                requiredGrant: 'editGrant'
                            }]
                        }),
                        new Tine.widgets.activities.ActivitiesPanel({
                            app: 'Addressbook',
                            showAddNoteForm: false,
                            border: false,
                            bodyStyle: 'border:1px solid #B5B8C8;'
                        }),
                        new Tine.widgets.tags.TagPanel({
                            app: 'Addressbook',
                            border: false,
                            bodyStyle: 'border:1px solid #B5B8C8;'
                        })
                    ]
                }]
            }, this.mapPanel,
            new Tine.widgets.activities.ActivitiesTabPanel({
                app: this.appName,
                record_id: (this.record && ! this.copyRecord) ? this.record.id : '',
                record_model: this.appName + '_Model_' + this.recordClass.getMeta('modelName')
            })
            ]
        };
    },
    
    /**
     * init component
     */
    initComponent: function () {
        var relatedRecords = {};
        
        this.initToolbar();
        
        this.supr().initComponent.apply(this, arguments);
    },
    
    /**
     * initToolbar
     */
    initToolbar: function() {
        var exportContactButton = new Ext.Action({
            id: 'exportButton',
            text: Tine.Tinebase.appMgr.get('Addressbook').i18n._('Export as pdf'),
            handler: this.onExportContact,
            iconCls: 'action_exportAsPdf',
            disabled: false,
            scope: this
        });
        var addNoteButton = new Tine.widgets.activities.ActivitiesAddButton({});
        this.parseAddressButton = new Ext.Action({
            text: Tine.Tinebase.appMgr.get('Addressbook').i18n._('Parse address'),
            handler: this.onParseAddress,
            iconCls: 'action_parseAddress',
            disabled: false,
            scope: this,
            enableToggle: true
        });
        
        this.tbarItems = [exportContactButton, addNoteButton, this.parseAddressButton];
    },
    
    /**
     * checks if form data is valid
     * 
     * @return {Boolean}
     */
    isValid: function () {
        var form = this.getForm();
        var isValid = true;
        
        // you need to fill in one of: n_given n_family org_name
        // @todo required fields should depend on salutation ('company' -> org_name, etc.) 
        //       and not required fields should be disabled (n_given, n_family, etc.) 
        if (form.findField('n_family').getValue() === '' && form.findField('org_name').getValue() === '') {
            var invalidString = String.format(this.app.i18n._('Either {0} or {1} must be given'), this.app.i18n._('Last Name'), this.app.i18n._('Company'));
            
            form.findField('n_family').markInvalid(invalidString);
            form.findField('org_name').markInvalid(invalidString);
            
            isValid = false;
        }
        
        return isValid && Tine.Addressbook.ContactEditDialog.superclass.isValid.apply(this, arguments);
    },
    
    /**
     * overwrites the isValid method on multipleEdit
     */
    isMultipleValid: function() {
        var isValid = true;
        if (((this.getForm().findField('n_family').getValue() === '') && (this.getForm().findField('n_family').edited)) 
            && ((this.getForm().findField('org_name').getValue() === '') && (this.getForm().findField('org_name').edited))) {
            var invalidString = String.format(this.app.i18n._('Either {0} or {1} must be given'), this.app.i18n._('Last Name'), this.app.i18n._('Company'));
            this.getForm().findField('n_family').markInvalid(invalidString);
            this.getForm().findField('org_name').markInvalid(invalidString);
            
            isValid = false;
        }
        
        return isValid;
    },
    
    /**
     * export pdf handler
     */
    onExportContact: function () {
        var downloader = new Ext.ux.file.Download({
            params: {
                method: 'Addressbook.exportContacts',
                filter: Ext.encode([{field: 'id', operator: 'in', value: this.record.id}]),
                options: Ext.util.JSON.encode({
                    format: 'pdf'
                })
            }
        });
        downloader.start();
    },
    
    /**
     * parse address handler
     * 
     * opens message box where user can paste address
     * 
     * @param {Ext.Button} button
     */
    onParseAddress: function (button) {
        if (button.pressed) {
            Ext.Msg.prompt(this.app.i18n._('Paste address'), this.app.i18n._('Please paste an address that should be parsed:'), function(btn, text) {
                if (btn == 'ok'){
                    this.parseAddress(text);
                } else if (btn == 'cancel') {
                    button.toggle();
                }
            }, this, 100);
        } else {
            button.setText(this.app.i18n._('Parse address'));
            this.tokenModePlugin.endTokenMode();
        }
    },
    
    /**
     * send address to server + fills record/form with parsed data + adds unrecognizedTokens to description box
     * 
     * @param {String} address
     */
    parseAddress: function(address) {
        Tine.log.debug('parsing address ... ');
        
        Tine.Addressbook.parseAddressData(address, function(result, response) {
            Tine.log.debug('parsed address:');
            Tine.log.debug(result);
            
            // only set the fields that could be detected
            Ext.iterate(result.contact, function(key, value) {
                this.record.set(key, value);
            }, this);
            
            var oldNote = (this.record.get('note')) ? this.record.get('note') : '';
            this.record.set('note', result.unrecognizedTokens.join(' ') + oldNote);
            this.onRecordLoad();
            
            this.parseAddressButton.setText(this.app.i18n._('End token mode'));
            this.tokenModePlugin.startTokenMode();
        }, this);
    },
    
    /**
     * onRecordLoad
     */
    onRecordLoad: function () {
        // NOTE: it comes again and again till 
        if (this.rendered) {
            var container = this.record.get('container_id');
            
            // handle default container
            // TODO is this still needed? don't we already have generic default container handling?
            if (! this.record.id) {
                if (this.forceContainer) {
                    container = this.forceContainer;
                    // only force initially!
                    this.forceContainer = null;
                } else if (! Ext.isObject(container)) {
                    container = Tine.Addressbook.registry.get('defaultAddressbook');
                }
                
                this.record.set('container_id', '');
                this.record.set('container_id', container);
            }
            
            if (this.mapPanel instanceof Tine.Addressbook.MapPanel) {
                this.mapPanel.onRecordLoad(this.record);
            }
        }
        this.supr().onRecordLoad.apply(this, arguments);
    }
});

/**
 * Opens a new contact edit dialog window
 * 
 * @return {Ext.ux.Window}
 */
Tine.Addressbook.ContactEditDialog.openWindow = function (config) {
    
    // if a container is selected in the tree, take this as default container
    var treeNode = Ext.getCmp('Addressbook_Tree') ? Ext.getCmp('Addressbook_Tree').getSelectionModel().getSelectedNode() : null;
    if (treeNode && treeNode.attributes && treeNode.attributes.container.type) {
        config.forceContainer = treeNode.attributes.container;
    } else {
        config.forceContainer = null;
    }
    
    var id = (config.record && config.record.id) ? config.record.id : 0;
    var window = Tine.WindowFactory.getWindow({
        width: 800,
        height: 610,
        name: Tine.Addressbook.ContactEditDialog.prototype.windowNamePrefix + id,
        contentPanelConstructor: 'Tine.Addressbook.ContactEditDialog',
        contentPanelConstructorConfig: config
    });
    return window;
};
/*
 * Tine 2.0
 * contacts combo box and store
 * 
 * @package     Addressbook
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */

Ext.ns('Tine.Addressbook');

/**
 * contact selection combo box
 * 
 * @namespace   Tine.Addressbook
 * @class       Tine.Addressbook.SearchCombo
 * @extends     Ext.form.ComboBox
 * 
 * <p>Contact Search Combobox</p>
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
 * Create a new Tine.Addressbook.SearchCombo
 * 
 * TODO         add     forceSelection: true ?
 */
Tine.Addressbook.SearchCombo = Ext.extend(Tine.Tinebase.widgets.form.RecordPickerComboBox, {
    
    /**
     * @cfg {Boolean} userOnly
     */
    userOnly: false,
    
    /**
     * @property additionalFilters
     * @type Array
     */
    additionalFilters: null,
    
    /**
     * use account objects/records in get/setValue
     * 
     * @cfg {Boolean} legacy
     * @legacy
     * 
     * TODO remove this later
     */
    useAccountRecord: false,
    allowBlank: false,
    
    itemSelector: 'div.search-item',
    minListWidth: 350,
    
    //private
    initComponent: function(){
        this.recordClass = Tine.Addressbook.Model.Contact;
        this.recordProxy = Tine.Addressbook.contactBackend;

        this.initTemplate();
        Tine.Addressbook.SearchCombo.superclass.initComponent.call(this);
    },
    
    /**
     * is called in accountMode to reset the value
     * @param value
     */
    processValue: function(value) {
        if (this.useAccountRecord) {
            if(value == '') {
                this.accountId = null;
                this.selectedRecord = null;
            }
        }
        return Tine.Addressbook.SearchCombo.superclass.processValue.call(this, value);
    },

    /**
     * use beforequery to set query filter
     * 
     * @param {Event} qevent
     */
    onBeforeQuery: function(qevent){
        Tine.Addressbook.SearchCombo.superclass.onBeforeQuery.apply(this, arguments);
        
        var filter = this.store.baseParams.filter;
        
        if (this.userOnly) {
            filter.push({field: 'type', operator: 'equals', value: 'user'});
        }
        
        if (this.additionalFilters !== null && this.additionalFilters.length > 0) {
            for (var i = 0; i < this.additionalFilters.length; i++) {
                filter.push(this.additionalFilters[i]);
            }
        }
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
                            '<td width="30%"><b>{[this.encode(values.n_fileas)]}</b><br/>{[this.encode(values.org_name)]}</td>',
                            '<td width="25%">{[this.encode(values.adr_one_street)]}<br/>',
                                '{[this.encode(values.adr_one_postalcode)]} {[this.encode(values.adr_one_locality)]}</td>',
                            '<td width="25%">{[this.encode(values.tel_work)]}<br/>{[this.encode(values.tel_cell)]}</td>',
                            '<td width="20%">',
                                '<img width="45px" height="39px" src="{jpegphoto}" />',
                            '</td>',
                        '</tr>',
                    '</table>',
                '</div></tpl>',
                {
                    encode: function(value) {
                         if (value) {
                            return Ext.util.Format.htmlEncode(value);
                        } else {
                            return '';
                        }
                    }
                }
            );
        }
    },
    
    getValue: function() {
        if (this.useAccountRecord) {
            if (this.selectedRecord) {
                return this.selectedRecord.get('account_id');
            } else {
                return this.accountId;
            }
        } else {
            return Tine.Addressbook.SearchCombo.superclass.getValue.call(this);
        }
    },

    setValue: function (value) {

        if (this.useAccountRecord) {
            if (value) {
                if(value.accountId) {
                    // account object
                    this.accountId = value.accountId;
                    value = value.accountDisplayName;
                } else if (typeof(value.get) == 'function') {
                    // account record
                    this.accountId = value.get('id');
                    value = value.get('name');
                }
            } else {
                this.accountId = null;
                this.selectedRecord = null;
                
            }
        }
        return Tine.Addressbook.SearchCombo.superclass.setValue.call(this, value);
    }

});

Ext.reg('addressbookcontactpicker', Tine.Addressbook.SearchCombo);
Tine.widgets.form.RecordPickerManager.register('Addressbook', 'Contact', Tine.Addressbook.SearchCombo);
/* 
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2007-2008 Metaways Infosystems GmbH (http://www.metaways.de)
 */

/*global Ext, Tine*/

Ext.ns('Tine.Addressbook');

Tine.Addressbook.MapPanel = Ext.extend(Ext.Panel, {
    
    /**
     * @property
     */    
    activeMapCard: null,
    
    record: null,
    
    layout: 'fit',
    frame: true,
    
    /**
     * Company address map panel
     * @type {Tine.widgets.MapPanel}
     */
    companyMap: null,
    
    /**
     * Private address map panel
     * @type {Tine.widgets.MapPanel}
     */
    privateMap: null,
    
    initComponent: function () {
        this.app = Tine.Tinebase.appMgr.get('Addressbook');
        
        this.title = this.app.i18n._('Map');

        this.defaults = {
            border: false
        };
        
        this.mapCards = new Ext.Panel({
            layout: 'card',
            activeItem: 0,
            items: []
        });

        this.idPrefix = Ext.id();
        
        this.tbar = [{
            id: this.idPrefix + 'tglbtn' + 'companyMap',
            enableToggle: true,
            allowDepress: false,
            text: this.app.i18n._('Company address'),
            handler: this.onMapChange.createDelegate(this, ['companyMap']),
            toggleGroup: this.idPrefix + 'maptglgroup'
        }, ' ', {
            id: this.idPrefix + 'tglbtn' + 'privateMap',
            enableToggle: true,
            allowDepress: false,
            text: this.app.i18n._('Private address'),
            handler: this.onMapChange.createDelegate(this, ['privateMap']),
            toggleGroup: this.idPrefix + 'maptglgroup'
        }];
        
        this.items = this.mapCards;
        
        Tine.Addressbook.MapPanel.superclass.initComponent.call(this);
    },
    
    /**
     * Change active map card
     * 
     * @param {String} map
     */
    onMapChange: function (map) {
        this.mapCards.layout.setActiveItem(this[map]);
        this.mapCards.layout.layout();
        this.activeMapCard = this[map];
    },
    
    /**
     * Called after contact record is loaded to fill map panels
     * 
     * @param {Tine.Addressbook.Model.Contact} record
     */
    onRecordLoad: function (record) {
        this.record = record;
        
        var adrOne = ! Ext.isEmpty(this.record.get('adr_one_lon')) && ! Ext.isEmpty(this.record.get('adr_one_lat')),
            adrTwo = ! Ext.isEmpty(this.record.get('adr_two_lon')) && ! Ext.isEmpty(this.record.get('adr_two_lat')),
            
            btnOne = Ext.getCmp(this.idPrefix + 'tglbtn' + 'companyMap'),
            btnTwo = Ext.getCmp(this.idPrefix + 'tglbtn' + 'privateMap');
        
           // if we have coordinates for company address add map panel
        if (adrOne && ! this.companyMap) {
            Tine.log.debug('Add company address map');
            this.companyMap = new Tine.widgets.MapPanel({
                map: 'companyMap',
                layout: 'fit',
                zoom: 15,
                listeners: {
                    scope: this,
                    'activate': function (p) {
                        if (! p.center) {
                            Tine.log.debug('Loading company address map coordinates: ' + this.record.get('adr_one_lon') + ', ' + this.record.get('adr_one_lat'));
                            p.setCenter(this.record.get('adr_one_lon'), this.record.get('adr_one_lat'));
                        }
                    }
                }
            });
            this.mapCards.add(this.companyMap);
            this.mapCards.doLayout();
        }
        
        // if we have coordinates for private address add map panel
        if (adrTwo && ! this.privateMap) {
            Tine.log.debug('Add private address map');
            this.privateMap = new Tine.widgets.MapPanel({
                map: 'privateMap',
                layout: 'fit',
                zoom: 15,
                listeners: {
                    scope: this,
                    'activate': function (p) {
                        if (! p.center) {
                            Tine.log.debug('Loading private address map coordinates: ' + this.record.get('adr_two_lon') + ', ' + this.record.get('adr_two_lat'));
                            p.setCenter(this.record.get('adr_two_lon'), this.record.get('adr_two_lat'));
                        }
                    }
                }
            });
            this.mapCards.add(this.privateMap);
            this.mapCards.doLayout();
        }
            
        btnOne.toggle(adrOne);
        btnOne.setDisabled(! adrOne);
        
        btnTwo.toggle(! adrOne && adrTwo);
        btnTwo.setDisabled(! adrTwo);
    },
    
    onRecordUpdate: function (record) {
    }
});
/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2011 Metaways Infosystems GmbH (http://www.metaways.de)
 */
Ext.ns('Tine.Addressbook');

/**
 * render the CardDAV Url into property panel of contianers
 * 
 * @class   Tine.Addressbook.CardDAVContainerPropertiesHookField
 * @extends Ext.form.TextField
 */
Tine.Addressbook.CardDAVContainerPropertiesHookField = Ext.extend(Ext.form.TextField, {

    anchor: '100%',
    readOnly: true,
    
    /**
     * @private
     */
    initComponent: function() {
        this.on('added', this.onContainerAdded, this);

        Tine.Addressbook.CardDAVContainerPropertiesHookField.superclass.initComponent.call(this);
    },
    
    /**
     * @private
     */
    onContainerAdded: function() {
        this.app = Tine.Tinebase.appMgr.get('Addressbook');
        this.fieldLabel = this.app.i18n._('CardDAV URL');
        
        this.propertiesDialog = this.findParentBy(function(p) {return !! p.grantContainer});
        this.grantContainer = this.propertiesDialog.grantContainer;
        
        if (this.grantContainer.application_id && this.grantContainer.application_id.name) {
            this.isAddressbook = (this.grantContainer.application_id.name == 'Addressbook');
        } else {
            this.isAddressbook = this.grantContainer.application_id === this.app.id;
        }
        
        this.hidden = ! this.isAddressbook;
        // cardDAV URL
        this.value = [
            window.location.href.replace(/\/?(index\.php.*)?$/, ''),
            '/addressbooks/',
            Tine.Tinebase.registry.get('currentAccount').contact_id,
            '/',
            this.grantContainer.id
        ].join('');
    }
    
});

Ext.ux.ItemRegistry.registerItem('Tine.widgets.container.PropertiesDialog.FormItems.Properties', Tine.Addressbook.CardDAVContainerPropertiesHookField, 100);