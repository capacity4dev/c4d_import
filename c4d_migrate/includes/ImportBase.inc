<?php

class C4dMigrateImportBase extends Migration {

  public function __construct($arguments) {
    parent::__construct($arguments);

    $this->setUpConnection();
    // Test connection.
    $this->getConnection();
  }

  /**
   * Private helper function to inform drupal about our d6 connection.
   */
  private function setUpConnection() {
    $c4d_database = array(
      'database' => variable_get(C4D_MIGRATE_DB_NAME, ''),
      'username' => variable_get(C4D_MIGRATE_DB_USER, ''),
      'password' => variable_get(C4D_MIGRATE_DB_PASS, ''),
      'host'     => variable_get(C4D_MIGRATE_DB_HOST, ''),
      'driver'   => 'mysql',
    );

    try {
      Database::addConnectionInfo(C4D_MIGRATE_DATABASE, 'default', $c4d_database);
    }
    catch (Exception $e) {
      drupal_set_message(
        t('Unable to connect to the database, please check your settings at <a href="@c4d_migrate_settings_url">@c4d_migrate_settings_url_plain</a>',
          array(
            '@c4d_migrate_settings_url'       => url('admin/content/migrate/import'),
            '@c4d_migrate_settings_url_plain' => 'admin/content/migrate/import'
          )
        ),
        'error'
      );
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

  /**
   * Test if our second database connection actually works.
   *
   * @return bool|DatabaseConnection
   *   Is the connection test successful or not.
   */
  public function getConnection() {
    try {
      return Database::getConnection('default', C4D_MIGRATE_DATABASE);
    }
    catch (Exception $e) {
      drupal_set_message(
        t('Unable to connect to the database, please check your settings at <a href="@c4d_migrate_settings_url">@c4d_migrate_settings_url_plain</a>',
          array(
            '@c4d_migrate_settings_url'       => url('admin/content/migrate/import'),
            '@c4d_migrate_settings_url_plain' => 'admin/content/migrate/import'
          )
        ),
        'error'
      );

      throw new Exception($e);
    }
  }

  /**
   * {@inheritdoc}
   */
  public function prepareRow($row) {
    return parent::prepareRow($row);
  }
}
