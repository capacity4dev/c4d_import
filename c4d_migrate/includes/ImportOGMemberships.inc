<?php
/**
 * @file
 * Migrate class to import users' OG memberships.
 */

/**
 * Class C4dMigrateImportOGMemberships
 */
class C4dMigrateImportOGMemberships extends C4dMigrateImportBase {
  /**
   * Content creations need to be run before this one.
   *
   * @var array
   */
  public $dependencies = array(
    'C4dMigrateImportUsers',
    'C4dMigrateImportNodeGroup',
    'C4dMigrateImportNodeProject',
  );

  /**
   * Configure the object during creation.
   */
  public function __construct() {
    parent::__construct(
          MigrateGroup::getInstance(C4D_MIGRATE_MIGRATION_GROUP)
    );

    $this->description = t("Import user's OG memberships from our Drupal 6 database.");

    $query = $this->createSelect('og_uid', 'u')
                  ->fields('u');

    $query->rightJoin('node', 'n', 'n.nid = u.nid');
    $query->condition(db_or()->condition('n.type', 'group')
                      ->condition('n.type', 'project_group'));
    $query->orderBy('n.nid', 'ASC');

    $this->source = new MigrateSourceSQL($query, array(), NULL, array('map_joinable' => FALSE));

    // Create a map object for tracking the relationships between source rows.
    $key = array(
      'nid' => array(
        'type'        => 'int',
        'not null'    => TRUE,
        'description' => 'node ID of group.',
      ),
      'uid' => array(
        'type'        => 'int',
        'not null'    => TRUE,
        'description' => 'User ID of member.',
      ),
    );

    $destination_handler = new MigrateDestinationOGMembership();

    $this->map = new MigrateSQLMap(
      $this->machineName,
      $key,
      $destination_handler->getKeySchema()
    );

    $this->destination = $destination_handler;

    $this->addFieldMapping('created', 'created');
    $this->addFieldMapping('changed', 'changed');
    $this->addFieldMapping('is_admin', 'is_admin');
    $this->addFieldMapping('state', 'is_active');

    $this->addFieldMapping('group_type', NULL)->defaultValue('node');
    $this->addFieldMapping('gid', 'nid');

    $this->addFieldMapping('entity_type', NULL)->defaultValue('user');
    $this->addFieldMapping('etid', 'uid');
  }

  /**
   * {@inheritdoc}
   */
  public function prepareRow($row) {
    $row->state = !empty($row->state) ? OG_STATE_ACTIVE : OG_STATE_PENDING;

    $q    = $this->createSelect('node', 'n');
    $type = $q->fields('n', array('type'))->condition('nid', $row->nid, '=')
              ->execute()->fetchField();

    switch ($type) {
      case 'project_group':
        $row->nid = $this->handleSourceMigration(
                         'C4dMigrateImportNodeProject',
                           $row->nid,
                           NULL,
                           $this);
        break;

      case 'group':
        $row->nid = $this->handleSourceMigration(
                         'C4dMigrateImportNodeGroup',
                           $row->nid,
                           NULL,
                           $this);
        break;
    }

    $row->uid = $this->handleSourceMigration(
                     'C4dMigrateImportUsers',
                       $row->uid,
                       NULL,
                       $this);

    parent::prepareRow($row);
  }
}
