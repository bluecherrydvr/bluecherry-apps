TOPDIR := $(CURDIR)
STAGE  := $(CURDIR)/stage
BCMK   = $(MAKE) -f "$(TOPDIR)"/BCMK.conf

export TOPDIR STAGE BCMK

MAKEFLAGS += --no-print-directory -rR

SUBDIRS = misc lib server utils php5

O := $(CURDIR)

# Goal blacklist, required in order to support debhelper
goal-bl   = distclean realclean test check

goals     := $(filter-out $(goal-bl),$(sort all $(notdir $(MAKECMDGOALS))))
sub-goals := $(foreach g,$(goals),$(SUBDIRS:=/$g))

$(goals): %:$(foreach d,$(SUBDIRS),$d/%) FORCE ;

$(sub-goals): FORCE
	@mkdir -p "$O/$(@D)"
	@$(BCMK) -C "$O/$(@D)" -f $(TOPDIR)/$(@D)/BCMK $(@F)

$(SUBDIRS): %:%/all FORCE ;

FORCE: ;
