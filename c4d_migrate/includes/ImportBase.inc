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
   * General initialization of a Migration object.
   *
   * This implements the common functionality to import nodes from a flat table.
   *
   * @param MigrateGroup $group
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
            '@url'       => url('admin/content/migrate/import'),
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
   * @param string $tableName
   *   Name of the database table.
   * @param string $tableAlias
   *   Alias to give the database table.
   *
   * @return bool|SelectQueryInterface
   */
  public function createSelect($tableName, $tableAlias) {
    if ($conn = $this->getConnection()) {
      return $conn->select($tableName, $tableAlias);
    }
  }
}
