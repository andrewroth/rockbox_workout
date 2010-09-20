class CreateSetFunctions < ActiveRecord::Migration
  def self.up
    create_table :set_functions do |t|
      t.integer :set_id
      t.string :variable
      t.integer :variable_function_id

      t.timestamps
    end
  end

  def self.down
    drop_table :set_functions
  end
end
