class CreateFunctions < ActiveRecord::Migration
  def self.up
    create_table :functions do |t|
      t.string :type
      t.decimal :base
      t.decimal :inc
      t.decimal :round_to_nearest
      t.decimal :min_n
      t.decimal :max_n
      t.decimal :min_v
      t.decimal :max_v

      t.timestamps
    end
  end

  def self.down
    drop_table :functions
  end
end
