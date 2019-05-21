#!/bin/bash
../build/json_config/test_json_config -p \
    --config-file=\[\"/yuneta/development/yuneta/^gobj-ecosistema-v2/ghelpers/tests_ghelpers/json_config/xx.json\",\"/yuneta/development/yuneta/^gobj-ecosistema-v2/ghelpers/tests_ghelpers/json_config/xxx.json\"\] \
    '{"more_variable_config": {"key_from_cli": "pepe", "max_size": 2}}'
