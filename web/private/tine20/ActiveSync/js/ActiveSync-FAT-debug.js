/*!
 * Tine 2.0 - ActiveSync 
 * Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 * http://www.gnu.org/licenses/agpl.html AGPL Version 3
 */
/*
 * Tine 2.0
 *
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @copyright   Copyright (c) 2010-2012 Metaways Infosystems GmbH (http://www.metaways.de)
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 */
Ext.ns('Tine.ActiveSync');

/**
 * @namespace   Tine.ActiveSync
 * @class       Tine.ActiveSync.Application
 * @extends     Tine.Tinebase.Application
 * 
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 */
Tine.ActiveSync.Application = Ext.extend(Tine.Tinebase.Application, {
    
    hasMainScreen: false,
    
    /**
     * Get translated application title of the calendar application
     * 
     * @return {String}
     */
    getTitle: function() {
        return this.i18n.gettext('Active Sync');
    },
    
    /**
     * returns additional items for persistent filter context menu
     * 
     * @todo rework this to be event/hook 
     * 
     * @param {Tine.widgets.persistentfilter.PickerPanel} picker
     * @param {Tine.widgets.persistentfilter.model.PersistentFilter} filter
     */
    getPersistentFilterPickerCtxItems: function(picker, filter) {
        var items = [];
        
        if (picker.app.appName.match(/Addressbook|Calendar|Email|Tasks/)) {
            var devices =  Tine.ActiveSync.getDeviceStore();
            var menuItems = ['<b class="x-menu-title">' + this.i18n._('Select a Device') +'</b>'];
            
            devices.each(function(device) {
                var contentClass = Tine.ActiveSync.Model.getContentClass(picker.app.appName);
                
                menuItems.push({
                    text: Ext.util.Format.htmlEncode(device.getTitle()),
                    checked: device.get([Ext.util.Format.lowercase(contentClass) + 'filter_id']) === filter.id,
                    //iconCls: 'activesync-device-standard',
                    handler: this.setDeviceContentFilter.createDelegate(this, [device, contentClass, filter], true)
                });
            }, this);
            if (! devices.getCount()) {
                menuItems.push({
                    text: this.i18n._('No ActiveSync Device registered'),
                    disabled: true,
                    checked: false,
                    handler: Ext.emptyFn
                });
            }
            
            items.push({
                text: String.format(this.i18n._('Set as {0} Filter'), this.getTitle()),
                iconCls: this.getIconCls(),
                menu: menuItems
            });
        }
        
        return items;
        
    },
    
    /**
     * persistently set filter for device
     * 
     * @param {Ext.Action} btn
     * @param {Ext.EventObject} e
     * @param {Tine.ActiveSync.Model.Device} device
     * @param {} contentClass
     * @param {Tine.widgets.persistentfilter.model.PersistentFilter} filter
     */
    setDeviceContentFilter: function(btn, e, device, contentClass, filter) {
        if (btn.checked) {
            // if btn was checked, we need to reset filter
            Tine.ActiveSync.setDeviceContentFilter(device.id, contentClass, null, function(response) {
                device.set([Ext.util.Format.lowercase(contentClass) + 'filter_id'], null);
                
                Ext.Msg.alert(this.i18n._('Resetted Sync Filter'), String.format(
                    this.i18n._('{0} filter for device "{1}" is now "{2}"'),
                        this.getTitle(),
                        Ext.util.Format.htmlEncode(device.getTitle()),
                        this.i18n._('resetted')
                ));
                
            }, this);
        } else {
            Tine.ActiveSync.setDeviceContentFilter(device.id, contentClass, filter.id, function(response) {
                device.set([Ext.util.Format.lowercase(contentClass) + 'filter_id'], filter.id);
                
                Ext.Msg.alert(this.i18n._('Set Sync Filter'), String.format(
                    this.i18n._('{0} filter for device "{1}" is now "{2}"'),
                        this.getTitle(),
                        Ext.util.Format.htmlEncode(device.getTitle()),
                        Ext.util.Format.htmlEncode(filter.get('name'))
                ));
            }, this);
        }
    }
});
/*
 * Tine 2.0
 *
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @copyright   Copyright (c) 2010-2012 Metaways Infosystems GmbH (http://www.metaways.de)
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 */
Ext.ns('Tine.ActiveSync.Model');


/**
 * @namespace   Tine.ActiveSync.Model
 * @class       Tine.ActiveSync.Model.Device
 * @extends     Tine.Tinebase.data.Record
 * 
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * Device record definition
 */
Tine.ActiveSync.Model.Device = Tine.Tinebase.data.Record.create(Tine.Tinebase.Model.genericFields.concat([
    { name: 'id' },
    { name: 'deviceid'},
    { name: 'devicetype' },
    { name: 'policykey' },
    { name: 'owner_id' },
    { name: 'acsversion' },
    { name: 'useragent' },
    { name: 'policy_id' },
    { name: 'pinglifetime', type: 'number' },
    { name: 'remotewipe', type: 'number' },
    { name: 'pingfolder' },
    { name: 'model' },
    { name: 'imei' },
    { name: 'friendlyname' },
    { name: 'os'},
    { name: 'oslanguage' },
    { name: 'phonenumber' }
]), {
    appName: 'ActiveSync',
    modelName: 'Device',
    idProperty: 'id',
    titleProperty: 'friendlyname',
    // ngettext('Device', 'Devices', n); gettext('Devices');
    recordName: 'Device',
    recordsName: 'Devices',
    
    /**
     * returns title of this record
     * 
     * @return {String}
     */
    getTitle: function() {
        return this.get('friendlyname') || this.get('useragent');
    }
});


/**
 * @namespace   Tine.ActiveSync.Model
 * @class       Tine.ActiveSync.Model.Device
 * @extends     Tine.Tinebase.data.Record
 * 
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * 
 * get content type of app
 * 
 * @static
 * @param {String} appName
 * @return {String}
 */
Tine.ActiveSync.Model.getContentClass = function(appName) {
    switch(appName) {
        case 'Calendar'   : return 'Calendar';
        case 'Addressbook': return 'Contacts';
        case 'Felamimail' : return 'Email';
        case 'Tasks'      : return 'Tasks';
        default: throw new Ext.Error('no contentClass for this app');
    }
};

/**
 * @namespace   Tine.ActiveSync.Model
 * @class       Tine.ActiveSync.Model.DeviceJsonBackend
 * @extends     Tine.Tinebase.data.RecordProxy
 * 
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * JSON backend for devices
 */
Tine.ActiveSync.Model.DeviceJsonBackend = Ext.extend(Tine.Tinebase.data.RecordProxy, {
    
    /**
     * Creates a recuring event exception
     * 
     * @param {Tine.Calendar.Model.Event} event
     * @param {Boolean} deleteInstance
     * @param {Boolean} deleteAllFollowing
     * @param {Object} options
     * @return {String} transaction id
     */
    setDeviceContentFilter: function(device, contentClass, filterId) {
        options = options || {};
        options.params = options.params || {};
        options.beforeSuccess = function(response) {
            return [this.recordReader(response)];
        };
        
        var p = options.params;
        p.method = this.appName + '.setDeviceContentFilter';
        p.deviceId = event.data;
        p.contentClass = contentClass;
        p.filterId = filterId;
        
        return this.doXHTTPRequest(options);
    }
});
/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @copyright   Copyright (c) 2010-2012 Metaways Infosystems GmbH (http://www.metaways.de)
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 */
Ext.ns('Tine.ActiveSync');

/**
 * @namespace   Tine.ActiveSync
 * @class       Tine.ActiveSync.DeviceStore
 * @extends     Ext.data.ArrayStore
 * 
 * <p>Store for Device Records</p>
 * 
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.ActiveSync.DeviceStore
 */
Tine.ActiveSync.DeviceStore = Ext.extend(Ext.data.ArrayStore, {
    
});


/**
 * @namespace   Tine.ActiveSync
 * 
 * get store of all device records
 * 
 * @static
 * @sigleton
 * @return {DeviceStore}
 */
Tine.ActiveSync.getDeviceStore = function() {
    if (! Tine.ActiveSync.deviceStore) {
        // create store
        Tine.ActiveSync.deviceStore = new Tine.ActiveSync.DeviceStore({
            fields: Tine.ActiveSync.Model.Device.getFieldDefinitions(),
            sortInfo: {field: 'friendlyname', direction: 'ASC'}
        });
        
        var app = Tine.Tinebase.appMgr.get('ActiveSync'),
            registry = app ? app.getRegistry() : null
            recordsData = registry ? registry.get('userDevices') : [];
        
        // populate store
        Ext.each(recordsData, function(data) {
            var r = new Tine.ActiveSync.Model.Device(data);
            Tine.ActiveSync.deviceStore.addSorted(r);
        }, this);
    }
    
    return Tine.ActiveSync.deviceStore;
}
