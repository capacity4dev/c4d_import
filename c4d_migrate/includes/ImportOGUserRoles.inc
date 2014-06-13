<?php
/**
 * @file
 * Migrate class to import users' OG roles.
 */

/**
 * Class C4dMigrateImportOGUserRoles
 */
class C4dMigrateImportOGUserRoles extends C4dMigrateImportBase {
  /**
   * Content creations need to be run before this one.
   *
   * @var array
   */
  public $dependencies = array(
    'C4dMigrateImportUsers',
    'C4dMigrateImportNodeGroup',
    'C4dMigrateImportNodeProject',
    'C4dMigrateImportRoles',
  );

  protected $defaultRoles;

  /**
   * Configure the object during creation.
   */
  public function __construct() {
    $table_name = 'og_users_roles';

    parent::__construct(
          MigrateGroup::getInstance(C4D_MIGRATE_MIGRATION_GROUP)
    );

    $this->description = t('Update the group ID in user roles.');

    $query = $this->createSelect('og_users_roles', 'ogur');
    $query->innerJoin('og', 'og', 'ogur.gid = og.nid');

    $query->fields('ogur', array('uid', 'rid'));

    $query->addField('og', 'nid', 'gid');

    $key = array(
      'uid' => array(
        'type'     => 'int',
        'not null' => TRUE,
      ),
      'rid' => array(
        'type'     => 'int',
        'not null' => TRUE,
      ),
      'gid' => array(
        'type'     => 'int',
        'not null' => TRUE,
      ),
    );

    $this->map         = new MigrateSQLMap($this->machineName, $key, MigrateDestinationTable::getKeySchema($table_name));
    $this->source      = new MigrateSourceSQL($query);
    $this->destination = new MigrateDestinationTable($table_name);

    $this->addFieldMapping('uid', 'uid');
    $this->addFieldMapping('rid')
         ->sourceMigration('C4dMigrateImportRoles');
    $this->addFieldMapping('gid');
  }

  /**
   * Override Migration::preImport().
   */
  protected function preImport() {
    parent::preImport();
    $this->defaultRoles = og_roles('', '');
  }

  /**
   * Prepare a n entity object.
   *
   * @param object $row
   *   The data row we are importing.
   */
  public function prepare($entity, $row) {
    $group_type = 'node';
    $gid        = $row->gid;

    if (!$group = entity_load_single($group_type, $gid)) {
      // Some installations might have missing entities, so we don't assume
      // they exist.
      return;
    }
    // Get the bundle from the entity.
    list(, , $bundle) = entity_extract_ids($group_type, $group);

    // Get the per-bundle roles, and replace the role ID, with the
    // per-bundle role ID.
    $og_roles = og_roles($group_type, $bundle, $gid);
    if (!empty($this->defaultRoles[$row->rid]) && $rid = array_search($this->defaultRoles[$row->rid], $og_roles)) {
      // Assign the new role ID.
      $entity->rid = $rid;
    }

    $entity->group_type = $group_type;
    $entity->gid        = $row->gid;
  }

  /**
   * Override Migration::postImport().
   *
   * Remove the global roles. We didn't do so it in OgMigrateRoles as
   * we need it in this class, when calling og_roles().
   * Also, delete legacy tables.
   */
  protected function postImport() {
    if (!$this->isComplete() || !$this->defaultRoles) {
      return;
    }
    parent::postImport();

    foreach (array_keys($this->defaultRoles) as $rid) {
      og_role_delete($rid);
    }
  }

  /**
   * {@inheritdoc}
   */
  public function prepareRow($row) {
    $row->state = !empty($row->state) ? OG_STATE_ACTIVE : OG_STATE_PENDING;

    $q    = $this->createSelect('node', 'n');
    $type = $q->fields('n', array('type'))->condition('nid', $row->gid, '=')
              ->execute()->fetchField();

    switch ($type) {
      case 'project_group':
        $row->gid = $this->handleSourceMigration(
                         'C4dMigrateImportNodeProject',
                           $row->gid,
                           NULL,
                           $this);
        break;

      case 'group':
        $row->gid = $this->handleSourceMigration(
                         'C4dMigrateImportNodeGroup',
                           $row->gid,
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
