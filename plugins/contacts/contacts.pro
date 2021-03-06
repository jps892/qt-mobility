TEMPLATE = subdirs

CONFIG += ordered

include(../../common.pri)

symbian {
   #symbian contactsmodel,plugin,symbian is part of new contactsrv pkg
   !contains(S60_VERSION, 5.4) {
     !contains(S60_VERSION, 5.5) {

        SUBDIRS += symbian

        ## build symbian-specific unit tests.
        contains(build_unit_tests, yes):SUBDIRS += symbian/plugin/tsrc

        # SIM backend depends on etel MM APIs
        contains(symbiancntsim_enabled, yes) {
            SUBDIRS += symbiansim

            ## build symbiansim-specific unit tests.
            contains(build_unit_tests, yes):SUBDIRS += symbiansim/tsrc

            message("Symbian SIM backend enabled")
        } else {
            message("Symbian SIM backend disabled")
        }
      }
    }
}
wince*:SUBDIRS += wince
maemo5 {
    contains(maemo5-contacts_enabled, yes): SUBDIRS += maemo5
}

contains(mobility_modules,serviceframework): SUBDIRS += serviceactionmanager

