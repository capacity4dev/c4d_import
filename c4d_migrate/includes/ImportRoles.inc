<?php
/**
 * @file
 * Migrate class to import users.
 */

/**
 * Class C4dMigrateImportRoles.
 */
class C4dMigrateImportRoles extends C4dMigrateImportBase {
  /**
   * Allow the implementor to map roles by name.
   *
   * Allow the implementor to map roles by name;
   * e.g., map the legacy "Fan Club" role to the destination
   * "fan club member" role.
   *
   * @var array
   */
  protected $roleMappings = array();

  public $entityType = 'role';

  /**
   * Configure the object during creation.
   */
  public function __construct() {

    parent::__construct(
          MigrateGroup::getInstance(C4D_MIGRATE_MIGRATION_GROUP)
    );

    $this->description = t('Import roles from our Drupal 6 database.');

    $query = $this->createSelect('role', 'r')
                  ->orderBy('rid', 'ASC')
                  ->fields('r')
                  ->condition('rid', 2, '>');

    $this->source = new MigrateSourceSQL($query, array(), NULL, array('map_joinable' => FALSE));

    // Create a map object for tracking the relationships between source rows.
    $key = array(
      'rid' => array(
        'type'     => 'varchar',
        'length'   => 255,
        'not null' => TRUE,
      ),
    );

    $this->roleMappings['anonymous user']     = 'anonymous user';
    $this->roleMappings['authenticated user'] = 'authenticated user';
    $this->roleMappings['site administrator'] = 'administrator';

    $destination_handler = new MigrateDestinationRole();

    $this->map = new MigrateSQLMap(
      $this->machineName,
      $key,
      $destination_handler->getKeySchema()
    );

    $this->destination = $destination_handler;

    $this->addFieldMapping('name', 'name')->dedupe('role', 'name');
  }

  /**
   * Prepare the role and check the mappings (for existing roles).
   *
   * @param object $role
   *   The role object we are saving.
   * @param object $row
   *   The data from the CSV file.
   *
   * @return bool
   *   Success.
   */
  public function prepare($role, $row) {
    if (isset($this->roleMappings[$role->name])) {
      $destination_name = $this->roleMappings[$role->name];
      $destination_rid  = db_select('role', 'r')
                          ->fields('r', array('rid'))
                          ->condition('name', $destination_name)
                          ->execute()
                          ->fetchField();
      if ($destination_rid) {
        $role->rid = $destination_rid;
        // Make sure we don't change the destination name.
        $role->name = $destination_name;
      }
    }

    return TRUE;
  }

  /**
   * Make sure not to delete pre-existing (mapped) roles.
   *
   * Override of Migration::rollback() - make sure we don't delete pre-existing
   * roles we've mapped to.
   *
   * @return int
   *   Success
   */
  protected function rollback() {
    $return = MigrationBase::RESULT_COMPLETED;
    $idlist = $this->getOption('idlist');
    if ($idlist) {
      // Make the IDs keys, to more easily identify them.
      $idlist = array_flip(explode(',', $idlist));
    }

    // Easily lookup destination role names.
    $roles_to_skip = array_flip($this->roleMappings);

    foreach ($this->map as $destination_key) {
      if ($this->timeOptionExceeded()) {
        break;
      }
      if (($return = $this->checkStatus()) != MigrationBase::RESULT_COMPLETED) {
        break;
      }
      if ($this->itemOptionExceeded()) {
        break;
      }
      $this->currentSourceKey = $this->map->getCurrentKey();

      // If there's an idlist, skip anything not in the list.
      if ($idlist && !isset($idlist[$this->currentSourceKey['sourceid1']])) {
        continue;
      }

      try {
        // Skip when the destination key is null.
        $skip      = FALSE;
        $key_value = reset($destination_key);
        if (is_null($key_value)) {
          $skip = TRUE;
        }
        // Or if the destination is pre-existing (a value in roleMappings).
        $destination_name = db_select('role', 'r')
                            ->fields('r', array('name'))
                            ->condition('rid', $key_value)
                            ->execute()
                            ->fetchField();
        if (isset($roles_to_skip[$destination_name])) {
          $skip = TRUE;
        }
        if (!$skip) {
          migrate_instrument_start('destination rollback');
          $this->destination->rollback((array) $destination_key);
          migrate_instrument_stop('destination rollback');
        }

        migrate_instrument_start('rollback map/message update');
        $this->map->delete($this->currentSourceKey);
        migrate_instrument_stop('rollback map/message update');
        $this->total_successes++;
        $this->successes_since_feedback++;
      }
      catch (Exception $e) {
        // TODO: At least count failures.
        continue;
      }
      $this->total_processed++;
      $this->processed_since_feedback++;
    }

    $this->map->clearMessages();

    $this->progressMessage($return);

    return $return;
  }

}
