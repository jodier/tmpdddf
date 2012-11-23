/*
 * Tine 2.0 - ActiveSync 
 * Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 * http://www.gnu.org/licenses/agpl.html AGPL Version 3
 */
Ext.ns("Tine.ActiveSync");Tine.ActiveSync.Application=Ext.extend(Tine.Tinebase.Application,{hasMainScreen:false,getTitle:function(){return this.i18n.gettext("Active Sync")},getPersistentFilterPickerCtxItems:function(c,e){var b=[];if(c.app.appName.match(/Addressbook|Calendar|Email|Tasks/)){var a=Tine.ActiveSync.getDeviceStore();var d=['<b class="x-menu-title">'+this.i18n._("Select a Device")+"</b>"];a.each(function(g){var f=Tine.ActiveSync.Model.getContentClass(c.app.appName);d.push({text:Ext.util.Format.htmlEncode(g.getTitle()),checked:g.get([Ext.util.Format.lowercase(f)+"filter_id"])===e.id,handler:this.setDeviceContentFilter.createDelegate(this,[g,f,e],true)})},this);if(!a.getCount()){d.push({text:this.i18n._("No ActiveSync Device registered"),disabled:true,checked:false,handler:Ext.emptyFn})}b.push({text:String.format(this.i18n._("Set as {0} Filter"),this.getTitle()),iconCls:this.getIconCls(),menu:d})}return b},setDeviceContentFilter:function(b,f,d,a,c){if(b.checked){Tine.ActiveSync.setDeviceContentFilter(d.id,a,null,function(e){d.set([Ext.util.Format.lowercase(a)+"filter_id"],null);Ext.Msg.alert(this.i18n._("Resetted Sync Filter"),String.format(this.i18n._('{0} filter for device "{1}" is now "{2}"'),this.getTitle(),Ext.util.Format.htmlEncode(d.getTitle()),this.i18n._("resetted")))},this)}else{Tine.ActiveSync.setDeviceContentFilter(d.id,a,c.id,function(e){d.set([Ext.util.Format.lowercase(a)+"filter_id"],c.id);Ext.Msg.alert(this.i18n._("Set Sync Filter"),String.format(this.i18n._('{0} filter for device "{1}" is now "{2}"'),this.getTitle(),Ext.util.Format.htmlEncode(d.getTitle()),Ext.util.Format.htmlEncode(c.get("name"))))},this)}}});Ext.ns("Tine.ActiveSync.Model");Tine.ActiveSync.Model.Device=Tine.Tinebase.data.Record.create(Tine.Tinebase.Model.genericFields.concat([{name:"id"},{name:"deviceid"},{name:"devicetype"},{name:"policykey"},{name:"owner_id"},{name:"acsversion"},{name:"useragent"},{name:"policy_id"},{name:"pinglifetime",type:"number"},{name:"remotewipe",type:"number"},{name:"pingfolder"},{name:"model"},{name:"imei"},{name:"friendlyname"},{name:"os"},{name:"oslanguage"},{name:"phonenumber"}]),{appName:"ActiveSync",modelName:"Device",idProperty:"id",titleProperty:"friendlyname",recordName:"Device",recordsName:"Devices",getTitle:function(){return this.get("friendlyname")||this.get("useragent")}});Tine.ActiveSync.Model.getContentClass=function(a){switch(a){case"Calendar":return"Calendar";case"Addressbook":return"Contacts";case"Felamimail":return"Email";case"Tasks":return"Tasks";default:throw new Ext.Error("no contentClass for this app")}};Tine.ActiveSync.Model.DeviceJsonBackend=Ext.extend(Tine.Tinebase.data.RecordProxy,{setDeviceContentFilter:function(c,b,a){options=options||{};options.params=options.params||{};options.beforeSuccess=function(e){return[this.recordReader(e)]};var d=options.params;d.method=this.appName+".setDeviceContentFilter";d.deviceId=event.data;d.contentClass=b;d.filterId=a;return this.doXHTTPRequest(options)}});Ext.ns("Tine.ActiveSync");Tine.ActiveSync.DeviceStore=Ext.extend(Ext.data.ArrayStore,{});Tine.ActiveSync.getDeviceStore=function(){if(!Tine.ActiveSync.deviceStore){Tine.ActiveSync.deviceStore=new Tine.ActiveSync.DeviceStore({fields:Tine.ActiveSync.Model.Device.getFieldDefinitions(),sortInfo:{field:"friendlyname",direction:"ASC"}});var b=Tine.Tinebase.appMgr.get("ActiveSync"),a=b?b.getRegistry():null;recordsData=a?a.get("userDevices"):[];Ext.each(recordsData,function(d){var c=new Tine.ActiveSync.Model.Device(d);Tine.ActiveSync.deviceStore.addSorted(c)},this)}return Tine.ActiveSync.deviceStore};