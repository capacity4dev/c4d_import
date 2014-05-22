<?php
/**
 * @file
 * Migrate class to import users.
 */

/**
 * Class C4dMigrateImportUsers
 */
class C4dMigrateImportUsers extends C4dMigrateImportBase {
  public $entityType = 'user';

  /**
   * Configure the object during creation.
   */
  public function __construct() {
    parent::__construct(
          MigrateGroup::getInstance(C4D_MIGRATE_MIGRATION_GROUP)
    );

    $this->description = t('Import users from our Drupal 6 database.');

    $query        = Database::getConnection('default', C4D_MIGRATE_DATABASE)
                            ->select('c4d_export_user_profile')
                            ->fields('c4d_export_user_profile',
        array(
          'uid',
          'name',
          'pass',
          'mail',
          'signature',
          'created',
          'access',
          'login',
          'status',
          'timezone',
          'language',
          'picture',
          'init',
          'data',
          // TBA : profile fields
        )
      );
    $this->source = new MigrateSourceSQL($query);

    // Create a map object for tracking the relationships between source rows.
    $key                 = array(
      'uid' => array(
        'type'     => 'varchar',
        'length'   => 255,
        'not null' => TRUE,
      ),
    );

    // Automatically convert drupal passwords to drupal7 hash.
    $destination_handler = new MigrateDestinationUser(array('md5_passwords' => TRUE));

    $this->map           = new MigrateSQLMap(
      $this->machineName,
      $key,
      $destination_handler->getKeySchema()
    );

    $this->destination = new MigrateDestinationUser();

    $this->addFieldMapping('name', 'name')->dedupe('users', 'name');
    $this->addFieldMapping('pass', 'pass');
    $this->addFieldMapping('mail', 'mail')->dedupe('users', 'mail');
    $this->addFieldMapping('signature', 'signature')->defaultValue('');
    $this->addFieldMapping('created', 'created');
    $this->addFieldMapping('access', 'access');
    $this->addFieldMapping('login', 'login');
    $this->addFieldMapping('status', 'status');
    $this->addFieldMapping('timezone', 'timezone_name')->defaultValue(NULL);
    $this->addFieldMapping('language', 'language')->defaultValue('');
    $this->addFieldMapping('picture', 'picture')->defaultValue(0);
    $this->addFieldMapping('init', 'init');
  }
}
