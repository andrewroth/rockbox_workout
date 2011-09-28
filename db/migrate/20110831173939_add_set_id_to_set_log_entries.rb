class AddSetIdToSetLogEntries < ActiveRecord::Migration
  def self.up
    add_column :set_log_entries, :set_id, :integer
  end

  def self.down
    remove_column :set_log_entries, :set_id
  end
end
