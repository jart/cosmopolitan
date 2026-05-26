#!/usr/bin/env python3
"""Generate psa_crypto_driver_wrappers.h
            psa_crypto_driver_wrappers_no_static.c

   This module is invoked by the build scripts to auto generate the
   psa_crypto_driver_wrappers.h and psa_crypto_driver_wrappers_no_static
   based on template files in script/data_files/driver_templates/.
"""
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

import sys
import os
import json
from typing import NewType, Dict, Any
from traceback import format_tb
import argparse
import jsonschema
import jinja2

import framework_scripts_path # pylint: disable=unused-import
from mbedtls_framework import build_tree

JSONSchema = NewType('JSONSchema', object)
# The Driver is an Object, but practically it's indexable and can called a dictionary to
# keep MyPy happy till MyPy comes with a more composite type for JsonObjects.
Driver = NewType('Driver', dict)


class JsonValidationException(Exception):
    def __init__(self, message="Json Validation Failed"):
        self.message = message
        super().__init__(self.message)


class DriverReaderException(Exception):
    def __init__(self, message="Driver Reader Failed"):
        self.message = message
        super().__init__(self.message)


def render(template_path: str, driver_jsoncontext: list) -> str:
    """
    Render template from the input file and driver JSON.
    """
    environment = jinja2.Environment(
        loader=jinja2.FileSystemLoader(os.path.dirname(template_path)),
        keep_trailing_newline=True)
    template = environment.get_template(os.path.basename(template_path))

    return template.render(drivers=driver_jsoncontext)

def generate_driver_wrapper_file(template_dir: str,
                                 output_dir: str,
                                 template_file_name: str,
                                 driver_jsoncontext: list) -> None:
    """
    Generate the file psa_crypto_driver_wrapper.c.
    """
    driver_wrapper_template_filename = \
        os.path.join(template_dir, template_file_name)

    result = render(driver_wrapper_template_filename, driver_jsoncontext)

    with open(file=os.path.join(output_dir, os.path.splitext(template_file_name)[0]),
              mode='w',
              encoding='UTF-8') as out_file:
        out_file.write(result)


def validate_json(driverjson_data: Driver, driverschema_list: dict) -> None:
    """
    Validate the Driver JSON against an appropriate schema
    the schema passed could be that matching an opaque/ transparent driver.
    """
    driver_type = driverjson_data["type"]
    driver_prefix = driverjson_data["prefix"]
    try:
        _schema = driverschema_list[driver_type]
        jsonschema.validate(instance=driverjson_data, schema=_schema)
    except KeyError as err:
        # This could happen if the driverjson_data.type does not exist in the provided schema list
        # schemas = {'transparent': transparent_driver_schema, 'opaque': opaque_driver_schema}
        # Print onto stdout and stderr.
        print("Unknown Driver type " + driver_type +
              " for driver " + driver_prefix, str(err))
        print("Unknown Driver type " + driver_type +
              " for driver " + driver_prefix, str(err), file=sys.stderr)
        raise JsonValidationException() from err

    except jsonschema.exceptions.ValidationError as err:
        # Print onto stdout and stderr.
        print("Error: Failed to validate data file: {} using schema: {}."
              "\n Exception Message: \"{}\""
              " ".format(driverjson_data, _schema, str(err)))
        print("Error: Failed to validate data file: {} using schema: {}."
              "\n Exception Message: \"{}\""
              " ".format(driverjson_data, _schema, str(err)), file=sys.stderr)
        raise JsonValidationException() from err


def load_driver(schemas: Dict[str, Any], driver_file: str) -> Any:
    """loads validated json driver"""
    with open(file=driver_file, mode='r', encoding='UTF-8') as f:
        json_data = json.load(f)
        try:
            validate_json(json_data, schemas)
        except JsonValidationException as e:
            raise DriverReaderException from e
        return json_data


def load_schemas(project_root: str) -> Dict[str, Any]:
    """
    Load schemas map
    """
    schema_file_paths = {
        'transparent': os.path.join(project_root,
                                    'scripts',
                                    'data_files',
                                    'driver_jsons',
                                    'driver_transparent_schema.json'),
        'opaque': os.path.join(project_root,
                               'scripts',
                               'data_files',
                               'driver_jsons',
                               'driver_opaque_schema.json')
    }
    driver_schema = {}
    for key, file_path in schema_file_paths.items():
        with open(file=file_path, mode='r', encoding='UTF-8') as file:
            driver_schema[key] = json.load(file)
    return driver_schema


def read_driver_descriptions(project_root: str,
                             json_directory: str,
                             jsondriver_list: str) -> list:
    """
    Merge driver JSON files into a single ordered JSON after validation.
    """
    driver_schema = load_schemas(project_root)

    with open(file=os.path.join(json_directory, jsondriver_list),
              mode='r',
              encoding='UTF-8') as driver_list_file:
        driver_list = json.load(driver_list_file)

    return [load_driver(schemas=driver_schema,
                        driver_file=os.path.join(json_directory, driver_file_name))
            for driver_file_name in driver_list]


def trace_exception(e: Exception, file=sys.stderr) -> None:
    """Prints exception trace to the given TextIO handle"""
    print("Exception: type: %s, message: %s, trace: %s" % (
        e.__class__, str(e), format_tb(e.__traceback__)
    ), file)


TEMPLATE_FILENAMES = ["psa_crypto_driver_wrappers.h.jinja",
                      "psa_crypto_driver_wrappers_no_static.c.jinja"]

def main() -> int:
    """
    Main with command line arguments.
    """
    def_arg_project_root = build_tree.guess_project_root()

    parser = argparse.ArgumentParser()
    parser.add_argument('--project-root', default=def_arg_project_root,
                        help='root directory of repo source code')
    parser.add_argument('--template-dir',
                        help='directory holding the driver templates')
    parser.add_argument('--json-dir',
                        help='directory holding the driver JSONs')
    parser.add_argument('output_directory', nargs='?',
                        help='output file\'s location')
    args = parser.parse_args()

    project_root = os.path.abspath(args.project_root)
    if build_tree.looks_like_mbedtls_root(project_root):
        project_root = os.path.join(project_root, 'tf-psa-crypto')

    crypto_core_directory = build_tree.crypto_core_directory(project_root)

    output_directory = args.output_directory if args.output_directory is not None else \
        crypto_core_directory

    template_directory = args.template_dir if args.template_dir is not None else \
        os.path.join(project_root,
                     'scripts',
                     'data_files',
                     'driver_templates')
    json_directory = args.json_dir if args.json_dir is not None else \
        os.path.join(project_root,
                     'scripts',
                     'data_files',
                     'driver_jsons')

    try:
        # Read and validate list of driver jsons from driverlist.json
        merged_driver_json = read_driver_descriptions(project_root,
                                                      json_directory,
                                                      'driverlist.json')
    except DriverReaderException as e:
        trace_exception(e)
        return 1
    for template_filename in TEMPLATE_FILENAMES:
        generate_driver_wrapper_file(template_directory, output_directory,
                                     template_filename, merged_driver_json)
    return 0


if __name__ == '__main__':
    sys.exit(main())
