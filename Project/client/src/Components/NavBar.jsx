import React from 'react'
import { Link as RouterLink } from 'react-router-dom'
import {
  AppBar,
  Button,
  Container,
  Divider,
  Stack,
  Toolbar,
  Typography,
} from '@mui/material'

function NavBar() {
  return (
    <AppBar
      position="sticky"
      elevation={0}
      sx={{
        background: '#9ca4c6e4',
        backdropFilter: 'none',
        border: 'none',
        borderRadius: '0px',
        color: 'common.white',
        boxShadow: 'none',
      }}
    >
      <Container maxWidth="lg">
        <Toolbar disableGutters sx={{ py: 1.5, minHeight: '88px' }}>
          <Stack
            direction="row"
            alignItems="center"
            justifyContent="space-between"
            sx={{ width: '100%' }}
          >
            <Typography
              variant="h4"
              fontWeight={700}
              sx={{
                letterSpacing: '0.08em',
                textTransform: 'uppercase',
              }}
            >
              Wat To Do
            </Typography>
            <Stack direction="row" spacing={1.5} alignItems="center">
              <Button
                component={RouterLink}
                to="/"
                color="inherit"
                sx={{ textTransform: 'none', fontWeight: 600, fontSize: '1rem' }}
              >
                Home
              </Button>
              <Divider orientation="vertical" flexItem sx={{ borderColor: 'rgba(255,255,255,0.3)' }} />
              <Button
                component={RouterLink}
                to="/profile"
                color="inherit"
                sx={{ textTransform: 'none', fontWeight: 600, fontSize: '1rem' }}
              >
                Profile
              </Button>
            </Stack>
          </Stack>
        </Toolbar>
      </Container>
    </AppBar>
  )
}

export default NavBar
