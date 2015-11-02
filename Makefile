TOPDIR := $(CURDIR)
STAGE  := $(CURDIR)/stage
CONTRIB_BASE_DIR := $(TOPDIR)/misc/contrib
BCMK   = $(MAKE) -f "$(TOPDIR)"/BCMK.conf -f BCMK

export TOPDIR STAGE BCMK CONTRIB_BASE_DIR

SUBDIRS = misc lib server utils php5


# Goal blacklist, required in order to support debhelper
goal-bl   = distclean realclean test check

goals     := $(filter-out $(goal-bl),$(sort all $(notdir $(MAKECMDGOALS))))
sub-goals := $(foreach g,$(goals),$(SUBDIRS:=/$g))

$(goals): %:$(foreach d,$(SUBDIRS),$d/%) FORCE ;

$(sub-goals): FORCE
	@$(BCMK) -C "$(@D)" $(@F)

$(SUBDIRS): %:%/all FORCE ;

FORCE: ;
