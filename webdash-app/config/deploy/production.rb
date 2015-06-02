
server 'nca-webdash.pineapplevisionsystems.com',
  user: 'deploy',
  roles: %w{web app db},
  ssh_options: {
    keys: [ File.join(ENV["HOME"], ".ssh", "nca-webdash-deploy") ],
    forward_agent: true,
    auth_methods: %w(publickey)
  }

namespace :deploy do
  namespace :db do

    desc "Create Production Database"
    task :create do
      on roles([:db]) do
        within "#{current_path}" do
          with rails_env: :production do
            rake "db:create"
          end
        end
      end
    end
    
    desc "Migrate Production Database"
    task :migrate do
      on roles([:db]) do
        within "#{current_path}" do
          with rails_env: :production do
            rake "db:migrate"
          end
        end
      end
    end 
    
    desc "Seed Production Database"
    task :seed do
      on roles([:db]) do
        within "#{current_path}" do
          with rails_env: :production do
            rake "db:seed"
          end
        end
      end
    end 
    
    desc "Drop Production Database"
    task :drop do
      on roles([:db]) do
        within "#{current_path}" do
          with rails_env: :production do
            rake "db:drop"
          end
        end
      end
    end 

  end
end

