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
    $this->addFieldMapping('field_picture', 'field_picture')
         ->defaultValue(NULL);
    $this->addFieldMapping('c4m_media', 'field_picture_file');
    $this->addFieldMapping('c4m_media:source_dir', 'field_picture_rootpath');
    $this->addFieldMapping('c4m_media:file_replace')
         ->defaultValue(FILE_EXISTS_REPLACE);

    // field_ref_topic -- Need Topic first
    // field_ref_topic_expertise -- Need Topic first
    $this->addFieldMapping('c4m_country', 'field_country');
    // group memberships -- Need Vocab_geo first
    $this->addFieldMapping('roles', 'roles')
         ->defaultValue(DRUPAL_AUTHENTICATED_RID);
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

    $row->data = unserialize($row->data);

    if (!empty($row->field_picture)) {
      // Retrieve actual file from fid.
      $f_select = $this->getConnection()->select('c4d_export_other_files', 'f');
      $f_select->fields('f');
      $f_select->condition('f.fid', $row->field_picture, '=');
      $res    = $f_select->execute();
      $result = $res->fetchObject();

      $row->field_picture_file = $result->filename;

      // All other keys combined are the relative path from the root dir.
      $rel_path = substr($result->filepath, 0, -strlen($result->filename));

      $row->field_picture_rootpath = variable_get(C4D_MIGRATE_FILES_ROOT, '/') . DIRECTORY_SEPARATOR . $rel_path;
//
//      $current_image_data = array(
//        'path'  => variable_get(C4D_MIGRATE_FILES_ROOT, '/') . DIRECTORY_SEPARATOR . $result->filepath,
//        'alt'   => '',
//        'title' => '',
//      );

//      $row->c4m_media = drupal_json_encode($current_image_data);
    }

    $roles = array();
    foreach (unserialize($row->roles) as $role) {
      $roles[] = $this->handleSourceMigration('C4dMigrateImportRoles', $role['rid'], NULL, $this);
    }
    $row->roles = $roles;

    return TRUE;
  }
}
