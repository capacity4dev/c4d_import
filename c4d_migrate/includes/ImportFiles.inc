<?php
/**
 * @file
 * Migrate class to import files.
 */

/**
 * Class C4dMigrateImportFiles
 */
class C4dMigrateImportFiles extends C4dMigrateImportBase {
  public $entityType = 'files';

  /**
   * Configure the object during creation.
   */
  public function __construct() {
    parent::__construct(
          MigrateGroup::getInstance(C4D_MIGRATE_MIGRATION_GROUP)
    );

    $this->description = t('Import files from our Drupal 6 database.');

    $query = $this->createSelect('c4d_export_other_files', 'f')
                  ->orderBy('fid', 'ASC')
                  ->fields('f');

    $this->source = new MigrateSourceSQL($query, array(), NULL, array('map_joinable' => FALSE));

    // Create a map object for tracking the relationships between source rows.
    $key = array(
      'fid' => array(
        'type'     => 'varchar',
        'length'   => 255,
        'not null' => TRUE,
      ),
    );

    // Automatically convert drupal passwords to drupal7 hash.
    $destination_handler = new MigrateDestinationFile('file', 'MigrateFileUri');

    $this->map = new MigrateSQLMap(
      $this->machineName,
      $key,
      $destination_handler->getKeySchema()
    );

    $this->destination = $destination_handler;

    $this->addFieldMapping('value', 'filepath');

    // Setup common mappings
    $this->addFieldMapping('destination_dir')
         ->defaultValue(variable_get('file_default_scheme', 'public') . '://');
    $this->addFieldMapping('source_dir')
         ->defaultValue(variable_get(C4D_MIGRATE_FILES_ROOT, '/'));
    $this->addFieldMapping('file_replace')
         ->defaultValue(FILE_EXISTS_REPLACE);
    $this->addFieldmapping('preserve_files')
         ->defaultValue(FALSE);
    $this->addFieldMapping('destination_file', 'filepath_stripped');
    $this->addFieldMapping('status', 'status');
    $this->addFieldMapping('timestamp', 'timestamp');
    $this->addFieldMapping('uid', 'uid')
//         ->sourceMigration('C4dMigrateImportUsers')
         ->defaultValue(1);
  }

  /**
   * {@inheritrow}
   */
  function prepareRow($row) {
    parent::prepareRow($row);
    $row->filepath_stripped = str_replace("sites/default/files/", "", $row->filepath);
  }


  /**
   * Create stub File (when creating content), which will be replaced afterwards.
   *
   * @param Migration $migration
   *   The migration that is being done to create this stub.
   * @param array $source_id
   *   Array of strings with the source id(s).
   *
   * @return array|bool
   *   Array with file id if successfully created, else FALSE.
   */
  public function createStub(Migration $migration, array $source_id) {
    $file = new stdClass();
    $file->filename = "stubfile.png";
    $file->uri = "public://sites/default/files/stubs/stubfile.png";
    $file->status = 0;

    file_save($file);

    if (isset($file->fid)) {
      return array($file->fid);
    }
    else {
      return FALSE;
    }
  }

}
