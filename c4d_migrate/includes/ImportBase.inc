<?php
/**
 * @file
 * Base class to import data into capacity4more.
 */

/**
 * Base class to import data from capacity4dev into capacity4more.
 */
abstract class C4dMigrateImportBase extends Migration {
  /**
   * Variable prefix to use to pass files from the row to the migration.
   *
   * @var string
   */
  const FILE_PREFIX_FILE = 'C4D_MIGRATE_FILE_FILE_';
  const FILE_PREFIX_PATH = 'C4D_MIGRATE_FILE_PATH_';


  /**
   * General initialization of a Migration object.
   *
   * This implements the common functionality to import nodes from a flat table.
   *
   * @param MigrateGroup $group
   *   The migrate Group object.
   */
  public function __construct($group = NULL) {
    // TODO: find out why we get an array instead of the expected MigrateGroup
    // object.
    if ($group instanceof MigrateGroup) {
      // OK : This is what the interface tells us what it should be!
    }
    elseif (!empty($group['group_name'])) {
      // We get an array and the group name is part of it.
      $group = MigrateGroup::getInstance($group['group_name']);
    }
    else {
      // Fallback if there is no group provided.
      $group = MigrateGroup::getInstance(C4D_MIGRATE_MIGRATION_GROUP);
    }
    parent::__construct($group);
  }

  /**
   * Private helper function to inform drupal about our d6 connection.
   *
   * This will add the DB connection parameters to the DB connections list.
   */
  private function setUpConnection() {
    $existing = Database::getConnectionInfo(C4D_MIGRATE_DATABASE);
    if (!is_null($existing)) {
      // Already setup, don't do it again.
      return;
    }

    // Add information based on the variables.
    $c4d_database = array(
      'database' => variable_get(C4D_MIGRATE_DB_NAME, ''),
      'username' => variable_get(C4D_MIGRATE_DB_USER, ''),
      'password' => variable_get(C4D_MIGRATE_DB_PASS, ''),
      'host'     => variable_get(C4D_MIGRATE_DB_HOST, ''),
      'driver'   => 'mysql',
    );
    Database::addConnectionInfo(C4D_MIGRATE_DATABASE, 'default', $c4d_database);
  }

  /**
   * Get the connection to the D6 database.
   *
   * @return DatabaseConnection
   *   The Database connection object.
   *
   * @throws Exception
   */
  public function getConnection() {
    try {
      $this->setUpConnection();
      return Database::getConnection('default', C4D_MIGRATE_DATABASE);
    }
    catch (Exception $e) {
      drupal_set_message(
        t('Unable to connect to the database, <a href="@url">please check your settings</a>.',
          array(
            '@url' => url('admin/content/migrate/import'),
          )
        ),
        'error'
      );

      // Bubble up the exception.
      throw $e;
    }
  }

  /**
   * Set the right database and return a selectQuery.
   *
   * @param string $name
   *   Name of the database table.
   * @param string $alias
   *   Alias to give the database table.
   *
   * @return bool|SelectQueryInterface
   *   The select object.
   */
  public function createSelect($name, $alias) {
    if ($conn = $this->getConnection()) {
      return $conn->select($name, $alias);
    }
  }

  /**
   * Helper to add mapping for a file.
   *
   * @param string $target
   *   The name of the field were the file needs to be stored.
   * @param string $source
   *   The name of the field that holds the file ID (fid) in the data row from
   *   the source table.
   */
  public function addFileFieldMapping($target, $source) {
    // File info is passed trough temporary variables.
    $file = self::FILE_PREFIX_FILE . $source;
    $path = self::FILE_PREFIX_PATH . $source;

    $this->addFieldMapping($target, $file);
    $this->addFieldMapping($target . ':source_dir', $path);
    $this->addFieldMapping($target . ':file_replace')
      ->defaultValue(FILE_EXISTS_REPLACE);
  }

  /**
   * Helper to prepare the field for a file field.
   *
   * This will query the file table from the D6 environment to retrieve the file
   * details and add it to the row. The mapping will use this data to fill the
   * file field on the new platform.
   *
   * @param object $row
   *   The data row.
   * @param string $source
   *   The name of the file source field.
   *   This field should contain a file id (fid).
   * @param string $table
   *   The table to use to lookup the file data.
   */
  public function prepareFileInRow($row, $source, $table = 'c4d_export_other_files') {
    // File info is passed trough temporary variables.
    $file = self::FILE_PREFIX_FILE . $source;
    $path = self::FILE_PREFIX_PATH . $source;

    // Prepare with empty values.
    $row->{$file} = NULL;
    $row->{$path} = NULL;

    // Check if any file data in the platform.
    if (empty($row->{$source})) {
      return;
    }

    // Retrieve actual file record from fid.
    $f_select = $this->getConnection()->select($table, 'f');
    $f_select->fields('f');
    $f_select->condition('f.fid', (int) $row->{$source});
    $res    = $f_select->execute();
    $result = $res->fetchObject();

    // Pass the filename.
    $row->{$file} = $result->filename;

    // All other keys combined are the relative path from the root dir.
    $rel_path = dirname($result->filepath);
    $row->{$path} = variable_get(C4D_MIGRATE_FILES_ROOT, '/')
      . DIRECTORY_SEPARATOR
      . $rel_path;
  }
}
