FROM fedora:33

# Actual jbofihe build needs
RUN yum -y install flex bison gcc perl
# Packaging
RUN yum -y install ruby-devel gcc make rpm-build libffi-devel
RUN gem install --no-document fpm
