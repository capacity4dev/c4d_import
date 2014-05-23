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
   * Content creations need to be run before this one.
   *
   * @var array
   */
  public $dependencies = array(
    'C4dMigrateImportRoles',
  );

  /**
   * Configure the object during creation.
   */
  public function __construct() {
    parent::__construct(
          MigrateGroup::getInstance(C4D_MIGRATE_MIGRATION_GROUP)
    );

    $this->description = t('Import users from our Drupal 6 database.');

    $query = $this->createSelect('c4d_export_user_profile', 'u')
                  ->orderBy('uid', 'ASC')
                  ->fields('u');

    $this->source = new MigrateSourceSQL($query, array(), NULL, array('map_joinable' => FALSE));

    // Create a map object for tracking the relationships between source rows.
    $key = array(
      'uid' => array(
        'type'     => 'varchar',
        'length'   => 255,
        'not null' => TRUE,
      ),
    );

    // Automatically convert drupal passwords to drupal7 hash.
    $destination_handler = new MigrateDestinationUser(array('md5_passwords' => TRUE));

    $this->map = new MigrateSQLMap(
      $this->machineName,
      $key,
      $destination_handler->getKeySchema()
    );

    $this->destination = $destination_handler;

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
    $this->addFieldMapping('data', 'data');
    $this->addFieldMapping('c4m_body', 'field_description');
    $this->addFieldMapping('c4m_first_name', 'field_firstname');
    $this->addFieldMapping('c4m_last_name', 'field_lastname');
    $this->addFieldMapping('c4m_organisation', 'field_organisation');
    $this->addFieldMapping('c4m_organisation_type', 'field_organisation_type_of');
    // field_picture
    // field_ref_topic
    // field_ref_topic_expertise
    $this->addFieldMapping('c4m_country', 'field_country');
    // group memberships
    $this->addFieldMapping('roles', 'roles')
         ->defaultValue(DRUPAL_AUTHENTICATED_RID)
         ->sourceMigration('C4dMigrateImportRoles');
  }

  /**
   * Prepare the row data loaded from the CSV.
   *
   * @param object $row
   *   The data from the CSV file.
   *
   * @return bool
   *   Success.
   */
  public function prepareRow($row) {
    parent::prepareRow($row);

    $row->data  = unserialize($row->data);
    $row->roles = array_keys(unserialize($row->roles));
  }
}
